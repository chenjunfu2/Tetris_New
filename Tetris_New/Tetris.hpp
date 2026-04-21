#pragma once

#include <stdint.h>
#include <array>


class TetrisBlock
{
private:
	std::array<uint8_t, 4> arrBlock[4] = { 0 };//4*4大小，4种旋转形式
	static constexpr uint8_t u16BitMask = 0b0000'1111;//低4bit掩模版

	uint8_t u8curRotation = 0;

protected:
	constexpr static uint8_t GetArrBit(const std::array<uint8_t, 4> &arrBase, size_t x, size_t y)
	{
		return (arrBase[y] >> x) & (uint8_t)0b0000'0001;
	}

	constexpr static void SetArrBit(std::array<uint8_t, 4> &arrBase, size_t x, size_t y, uint8_t u8Bit)
	{
		arrBase[y] &= (u8Bit & (uint8_t)0b0000'0001) << x;
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





	}







};


class TetrisBorad
{
private:
	std::array<uint16_t, 20> arrBorad = { 0 };//每个16bit中的10bit作为一行（宽10），一共20个元素（高20）
	static constexpr uint16_t u16BitMask = 0b0000'0011'1111'1111;//低10bit掩模版

public:
	bool CanFall()
	{

	}

















};
