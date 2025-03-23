#pragma once


#include "Claymore.h" // 클레이모어 장검 14장 (전체)
#include "Axe.h" // 도끼 11장 (전체)
#include "Magnum.h" // 6장
#include "Staff.h" // 17장(차징) 10장(차징 루프) 15장(발사)
#include "ShotGun.h" // 샷건 3장(발사) 14장(리로드)
#include "Minigun.h" //  총기사진 8장 왼손사진 3장
#include "Harvester.h" // 하베스터 27장
#include "Sonic.h" //7장

typedef struct Bullet_Base_INFO
{
	_float3 Player_RIght{};
	_float3 Player_Up{};
	_float3 Player_Look{};
	_float3 Player_Pos{};
}Player_INFO;