#pragma once

#include <stdint.h>
#include <array>


class TetrisBlock
{
public:
	static constexpr size_t szBlockSide = 4;
	static constexpr size_t szRotationCount = 4;
	static constexpr uint8_t u8BlockBitMask = 0b0000'1111;//低4bit掩模版
private:
	std::array<uint8_t, 2> arrBlock[szRotationCount] = { 0 };//4*4大小，4种旋转形式
	uint8_t u8curRotation = 0;

protected:
	constexpr static uint8_t GetArrBit(const std::array<uint8_t, 2> &arrBase, size_t x, size_t y)
	{
		//计算x和y的实际值
		x = y % 2 == 0 ? x : x + 4;
		y /= 2;

		return (arrBase[y] >> x) & (uint8_t)0b0000'0001;
	}

	constexpr static void SetArrBit(std::array<uint8_t, 2> &arrBase, size_t x, size_t y, uint8_t u8Bit)
	{
		//计算x和y的实际值
		x = y % 2 == 0 ? x : x + 4;
		y /= 2;

		arrBase[y] &= ~((uint8_t)0b0000'0001 << x);//先清除位
		arrBase[y] |= (u8Bit & (uint8_t)0b0000'0001) << x;//再设置位
	}

	constexpr static uint8_t GetArrLine(const std::array<uint8_t, 2> &arrBase, size_t y)
	{
		size_t szMove = y % 2 == 0 ? 0 : 4;
		return (arrBase[y] >> szMove) & u8BlockBitMask;
	}

	constexpr static std::array<uint8_t, 2> RotateArr(const std::array<uint8_t, 2> &arrBase, uint8_t u8Rotation)
	{
		std::array<uint8_t, 2> arrRotate = { 0 };

		u8Rotation %= szRotationCount;//szRotationCount->4个旋转方向，每个旋转是90度
		for (size_t y = 0; y < szBlockSide; ++y)
		{
			for (size_t x = 0; x < szBlockSide; ++x)
			{
				switch (u8Rotation)
				{
				case 0://直接拷贝
					SetArrBit(arrRotate, x, y, GetArrBit(arrBase, x, y));
					break;
				case 1://右转90
					SetArrBit(arrRotate, x, y, GetArrBit(arrBase, szBlockSide - 1 - x, y));
					break;
				case 2://翻转180
					SetArrBit(arrRotate, x, y, GetArrBit(arrBase, szBlockSide - 1 - x, szBlockSide - 1 - y));
					break;
				case 3://左转90
					SetArrBit(arrRotate, x, y, GetArrBit(arrBase, x, szBlockSide - 1 - y));
					break;
				default:
					return { 0 };//意外结果
					break;
				}
			}
		}

		return arrRotate;
	}

	constexpr static uint8_t HighNBits(uint8_t n)
	{
		return ((((uint8_t)0b0000'0001 << n) - 1) << (szBlockSide - n)) & 0b0000'1111;
	}

	constexpr static uint8_t LowNBits(uint8_t n)
	{
		return ((uint8_t)0b0000'0001 << n) - 1;
	}

public:
	constexpr TetrisBlock(const std::array<uint8_t, 2> &arrBlockBase) :
		arrBlock
		{
			RotateArr(arrBlockBase, 0),
			RotateArr(arrBlockBase, 1),
			RotateArr(arrBlockBase, 2),
			RotateArr(arrBlockBase, 3),
		},
		u8curRotation(0)
	{}


	const std::array<uint8_t, 2> &GetCurRotateBlock(void) const
	{
		return arrBlock[u8curRotation];
	}

	uint8_t GetCurRotation(void) const
	{
		return u8curRotation;
	}

	void RotateBlock(int8_t i8Rotation)
	{
		u8curRotation += (uint8_t)i8Rotation;
		u8curRotation %= 4;
	}

	uint8_t GetCurRotateBlockBit(size_t x, size_t y) const
	{
		return GetArrBit(GetCurRotateBlock(), x, y);
	}

	uint8_t GetCurRotateBlockLine(size_t y) const
	{
		return GetArrLine(GetCurRotateBlock(), y);
	}

	bool BoundaryTest(size_t szLowX, size_t szLowY, size_t szHighX, size_t szHighY) const
	{
		if (szLowX > szHighX ||
			szLowY > szHighY ||
			szHighX > szBlockSide ||
			szHighY > szBlockSide)
		{
			return false;//越界与上下边界要求检测
		}
		
		const auto &curBlock = GetCurRotateBlock();


		auto TraverseTestBlock =
		[&curBlock](size_t szYBeg, size_t szYEnd, uint8_t u8BoundaryMask) -> bool
		{
			for (size_t y = szYBeg; y < szYEnd; ++y)
			{
				if (curBlock[y] & u8BoundaryMask)//与边界mask与运算后不为0，那么说明原始内容存在边界相交
				{
					return false;
				}
			}

			return true;
		};

		return
		//遍历低y之前的部分，要求必须全为0
		TraverseTestBlock(0, szLowY, (uint8_t)0b0000'1111) && 
		//遍历中间覆盖部分，判断左右边界
		TraverseTestBlock(szLowY, szHighY, HighNBits(szBlockSide - szHighX) | LowNBits(szLowX)) &&
		//遍历高y之后的部分，要求必须全为0
		TraverseTestBlock(szHighY, szBlockSide, (uint8_t)0b0000'1111);
	}

};


class TetrisBorad
{
public:
	static constexpr size_t szBoradWide = 10;
	static constexpr size_t szBoradHigh = 20;
	static constexpr uint16_t u16BoradBitMask = 0b0000'0011'1111'1111;//低10bit掩模版
private:
	std::array<uint16_t, 20> arrBorad = { 0 };//每个16bit中的10bit作为一行（宽10），一共20个元素（高20）
	//高6bit存储这一行有几个方块，用于快速消除判断

public:
	bool CanMove(const TetrisBlock &csBlock, int64_t i64BlockX, int64_t i64BlockY)
	{
		//处理越界，使用位与判断是否有重叠部分
		if (i64BlockX >= szBoradWide ||
			i64BlockY >= szBoradHigh ||//正向超出边界大小
			i64BlockX <= -(int64_t)csBlock.szBlockSide ||
			i64BlockY <= -(int64_t)csBlock.szBlockSide)//负向超出一个方块大小
		{
			return false;
		}

		//先进行方块越界判断处理
		size_t szBlockLowX = i64BlockX >= 0 ? 0 : -i64BlockX;//如果没有负向溢出，那么从0判断，否则从溢出长度判断
		size_t szBlockLowY = i64BlockY >= -(int64_t)csBlock.szBlockSide ? 0 : -i64BlockY - (int64_t)csBlock.szBlockSide;//如果方块没有超出上面1个方块大小的距离（下落保留区域），那么使用0判断，否则裁剪方块起始边界长度为溢出保留区域的长度
		size_t szBlockHighX = i64BlockX + csBlock.szBlockSide <= (int64_t)szBoradWide ? csBlock.szBlockSide : (int64_t)szBoradWide - i64BlockX;//如果没有正向溢出，那么从结束边界判断，否则从还未溢出的长度判断
		size_t szBlockHighY = i64BlockY + csBlock.szBlockSide <= (int64_t)szBoradHigh ? csBlock.szBlockSide : (int64_t)szBoradHigh - i64BlockY;

		if (!csBlock.BoundaryTest(szBlockLowX, szBlockLowY, szBlockHighX, szBlockHighY))
		{
			return false;
		}

		//方块越界游戏区域在碰撞判断的同时处理
		size_t szBoardYBeg = i64BlockY < 0 ? 0 : i64BlockY;//直接从边界内开始判断，溢出部分必然不可能与合法方块碰撞
		size_t szBlockYBeg = szBlockLowY;
		for (size_t i = 0; i < szBlockHighY - szBlockLowY; ++i)//裁切到边界内
		{
			//因为每一行仅由bit位组成，可以一次性使用位与处理而不用for
			//对于游戏板内每一行，根据i64BlockX取出4bit，然后和方块进行位与

			//首先判断这一行是否有方块（数量为0）
			//如果没有直接跳过
			if ((uint8_t)(arrBorad[szBoardYBeg + i] >> szBoradWide) == 0)
			{
				continue;
			}

			//方块所在区域的位
			size_t szAreaWide = szBlockHighX - szBlockLowX;
			uint8_t u8BlockArea = ((arrBorad[szBoardYBeg + i] >> (szBoradWide - szAreaWide)) << (csBlock.szBlockSide - szAreaWide)) & csBlock.u8BlockBitMask;
			if (u8BlockArea & csBlock.GetCurRotateBlockLine(szBlockYBeg + i))//如果与不为0则碰撞
			{
				return false;
			}
		}

		return true;
	}

	//如果成功返回true，并设置方块旋转后（可能被挤压或移动）的新坐标，否则返回false
	bool CanRotate(const TetrisBlock &csBlock, int64_t &i64BlockX, int64_t &i64BlockY, int8_t i8NewRotation)
	{

	}

	//在地图中固化方块并消除满行，返回false代表固化失败（游戏结束）
	bool SolidifyAndClear(const TetrisBlock &csBlock, int64_t i64BlockX, int64_t i64BlockY)
	{

	}
};
