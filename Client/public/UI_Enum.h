﻿#pragma once

enum Button_type // 이 버튼은 어느 메뉴창의 버튼인지
{
    Episode = 0, Point_Shop = 1, Spell_Shop = 2, Upgrade_Weapon = 3
};

enum Level_01_Type // 이 버튼은 어느 이미지를 사용할것인지
{
    Level_ICon_Defaul, Level_ICon_Selected,
    Stage_01_Color_Default, Stage_01_Color_Selected,
    Stage_02_Gray_Default, Stage_02_Gray_Selected, Stage_02_Color_Default, Stage_02_Color_Selected,
    Stage_03_Gray_Default, Stage_03_Gray_Selected, Stage_03_Color_Default, Stage_03_Color_Selected,
    Stage_04_Gray_Default, Stage_04_Gray_Selected, Stage_04_Color_Default, Stage_04_Color_Selected,
    Stage_05_Gray_Default, Stage_05_Gray_Selected, Stage_05_Color_Default, Stage_05_Color_Selected,
    Episode_Display,

    Point_Shop_Display = 0, Point_Shop_Selected = 1, Point_Shop_Small_Selected = 2
};
enum Poinst_Shop_Button // 이 버튼은 어느 이미지를 사용할것인지
{
    AXE_POWER, BULLET_COLLECTOR, TREASURE_HUNTER, GHOST, HEAL_IS_GOOD, SUPER_BOY,
    STUDENT, MAGICIAN, ShotGun_MASTER, SOUL_COLLECTOR, FIRE_BOY, VERY_FASTER
};
