#include "graphics_engine/graphic_renderer.h"

/*
 * �����[  �����[���������������[�������������[  �����������[  �������������[�����[     ���������������[���������������[
 * �����U  �����U�����X�T�T�T�T�a�����X�T�T�����[�����X�T�T�����[�����X�T�T�T�T�a�����U     �����X�T�T�T�T�a�����X�T�T�T�T�a
 * ���������������U�����������[  �������������X�a���������������U�����U     �����U     �����������[  ���������������[
 * �����X�T�T�����U�����X�T�T�a  �����X�T�T�����[�����X�T�T�����U�����U     �����U     �����X�T�T�a  �^�T�T�T�T�����U
 * �����U  �����U���������������[�����U  �����U�����U  �����U�^�������������[���������������[���������������[���������������U
 * �^�T�a  �^�T�a�^�T�T�T�T�T�T�a�^�T�a  �^�T�a�^�T�a  �^�T�a �^�T�T�T�T�T�a�^�T�T�T�T�T�T�a�^�T�T�T�T�T�T�a�^�T�T�T�T�T�T�a
 */

using namespace heracles;

int main() {

	graphic_renderer::init();

	graphic_renderer::loop();

	return 0;
}