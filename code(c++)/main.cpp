#include "CospaceSettings.hpp"
#include "CommonTools.hpp"
#include "Hikaru.hpp"
#include <bits/stdc++.h>

// sprintf�̃G���[�𖳎����邽��
#define _CRT_SECURE_NO_WARNINGS 1

/*
	�����K����	�擪	��؂�	�K�p��
	PascalCase	��		��		�N���X��
	camelCase	��		��		���\�b�h���A�֐���
	snake_case	�S�ď�	_		�ϐ����A�����o���A�t�@�C����
	SNAKE_CASE	�S�đ�	_		�}�N�����A�萔�A�񋓑�
	�������ACospace�Ŋ��Ɍ��܂��Ă���ϐ��͏�ɏ]��Ȃ�(Cospace�Œ�܂��Ă�����̂ɓ����`���ɕϐ�����ύX����H)
	�܂��A�����Ȃǂ̊֌W�Ŏ��F���������Ƃ�������ɏ]��Ȃ��ėǂ�
*/
/*
	�C�����邱��
	�E�֐��̈������Ȃ��ꍇ�A������void�����邱�ƁB�������Ȃɂ��Ȃ��֐��Ɉ�����n���ƁA�S���ʂ̕ϐ��̒l�����������Bvoid������ƁA��������ꂽ���_�ŃG���[�\�����o��
	�E
*/

/*	�C���N���[�h�ɂ���
�w�b�_�t�@�C�����m���ˑ��֌W�������Ƃ��Ȃ��悤�ɂ���
![image](out/include_files/include_files.svg)
*/

using namespace std;

Game0_Hikaru game0;
Game1_Hikaru game1;

void Game0()
{
	if (getRepeatedNum() == 0)
	{
		commonSetup0();
		game0.setup();
	}
	game0.loop();
	commonLoop0();
}

void Game1()
{
	if (getRepeatedNum() == 0)
	{
		commonSetup1();
		game1.setup();
	}
	game1.loop();
	commonLoop1();
}
