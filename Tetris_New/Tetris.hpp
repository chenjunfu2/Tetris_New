#pragma once

#include <stdint.h>
#include <array>


class TetrisBlock
{
private:
	static constexpr uint8_t u8BlockBitMask = 0b0000'1111;//低4bit掩模版
	std::array<uint8_t, 4> arrBlock[4] = { 0 };//4*4大小，4种旋转形式
	uint8_t u8curRotation = 0;

protected:
	constexpr static uint8_t GetArrBit(const std::array<uint8_t, 4> &arrBase, size_t x, size_t y)
	{
		return (arrBase[y] >> x) & (uint8_t)0b0000'0001;
	}

	constexpr static void SetArrBit(std::array<uint8_t, 4> &arrBase, size_t x, size_t y, uint8_t u8Bit)
	{
		arrBase[y] &= ~((uint8_t)0b0000'0001 << x);//先清除位
		arrBase[y] |= (u8Bit & (uint8_t)0b0000'0001) << x;//再设置位
	}

	constexpr static std::array<uint8_t, 4> RotateArr(const std::array<uint8_t, 4> &arrBase, uint8_t u8Rotation)
	{
		std::array<uint8_t, 4> arrRotate = { 0 };
		size_t szArrSize = arrBase.size();

		u8Rotation %= 4;//4->4个旋转方向，每个旋转是90度
		for (size_t y = 0; y < szArrSize; ++y)
		{
			for (size_t x = 0; x < szArrSize; ++x)
			{
				switch (u8Rotation)
				{
				case 0://直接拷贝
					SetArrBit(arrRotate, x, y, GetArrBit(arrBase, x, y));
					break;
				case 1://右转90
					SetArrBit(arrRotate, x, y, GetArrBit(arrBase, szArrSize - 1 - x, y));
					break;
				case 2://翻转180
					SetArrBit(arrRotate, x, y, GetArrBit(arrBase, szArrSize - 1 - x, szArrSize - 1 - y));
					break;
				case 3://左转90
					SetArrBit(arrRotate, x, y, GetArrBit(arrBase, x, szArrSize - 1 - y));
					break;
				default:
					return { 0 };//意外结果
					break;
				}
			}
		}

		return arrRotate;
	}


public:
	constexpr TetrisBlock(const std::array<uint8_t, 4> &arrBlockBase)
	{
		for (size_t i = 0; i < arrBlockBase.size(); ++i)
		{
			arrBlock[0][i] = arrBlockBase[i] & u8BlockBitMask;//预处理输入并拷贝
		}

		//生成旋转
		arrBlock[1] = RotateArr(arrBlock[0], 1);
		arrBlock[2] = RotateArr(arrBlock[0], 2);
		arrBlock[3] = RotateArr(arrBlock[0], 3);
	}


	const std::array<uint8_t, 4> &GetCurRotateBlock(void) const
	{
		return arrBlock[u8curRotation];
	}

	uint8_t GetCurRotation(void) const
	{
		return u8curRotation;
	}

	void RotateBlock(uint8_t u8Rotation)
	{
		u8curRotation += u8Rotation;
		u8curRotation %= 4;
	}
};


class TetrisBorad
{
private:
	static constexpr uint16_t u16BoradBitMask = 0b0000'0011'1111'1111;//低10bit掩模版
	std::array<uint16_t, 20> arrBorad = { 0 };//每个16bit中的10bit作为一行（宽10），一共20个元素（高20）
	

public:
	bool CanMove(const TetrisBlock &csBlock, size_t x, size_t y)
	{
		//处理越界，使用位与判断是否有重叠部分
	}

















};
