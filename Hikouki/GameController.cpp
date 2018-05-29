#include "GameController.h"

GameController::GameController(HINSTANCE hinst, HWND hwnd, int _width, int _height, bool fullscreen)
	: under_controll(0), over_camera({ 0, 90, -100 }), view_type(CameraTypes::OVER)
{
	init(hinst, hwnd, _width, _height, fullscreen);
}

GameController::~GameController()
{
	uninit();
}

void GameController::init(HINSTANCE hinst, HWND hwnd, int _width, int _height, bool fullscreen)
{
	width = _width;
	height = _height;

	// ���͏�����
	input_device = new Input(hinst);
	keyboard = new Keyboard(input_device->get(), hwnd);
	mouse = new Mouse(input_device->get(), hwnd);

#ifdef _DEBUG
//	DebugConsole::create_console_window();
#endif

	graphics = new CDirectXGraphics();	// DirectX Graphics�I�u�W�F�N�g����

	if (!graphics->Init(hwnd, fullscreen, width, height)) {	// DirectX Graphics�I�u�W�F�N�g������
		throw "DirectX �������G���[";
	}

	constexpr float margin = 50.0f;

	camera = new Camera();

	xfile_manager = new XFileManager(graphics->GetDXDevice());
	xfile_manager->add({
		{ "Airplain", "assets/f1.x" }, // ��s�@
		{ "Skydome", "assets/skydome.x" } // �X�J�C�h�[��
		});

	skydome = new XFileObjectBase(xfile_manager->get("Skydome"));

	airplains.push_back(new Airplain(xfile_manager->get("Airplain"), graphics->GetDXDevice(), D3DXVECTOR3( 0.0, 0.0, 10.0f )));
	airplains.push_back(new Airplain(xfile_manager->get("Airplain"), graphics->GetDXDevice(), D3DXVECTOR3( 0.0, 0.0, -10.0f)));

	// �����ݒ�
	graphics->SetRenderStateArray({
		{ D3DRS_ZENABLE, TRUE }, // �y�o�b�t�@�L��
		{ D3DRS_LIGHTING, true }, // ���C�g�L��
		{ D3DRS_CULLMODE, D3DCULL_NONE }, // �J�����O������
		{ D3DRS_AMBIENT, 0xffffffff }, // �����Z�b�g
		{ D3DRS_ALPHABLENDENABLE, TRUE }, // �A���t�@�E�u�����f�B���O���s��
		{ D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA }, // ���ߏ������s��
		{ D3DRS_SRCBLEND, D3DBLEND_SRCALPHA } // �������������s��
		});

#ifdef _DEBUG
	debug_font = new CDebugFont();
	debug_font->CreateFontA(graphics->GetDXDevice());
#endif
	Start();
}

void GameController::uninit()
{
	Stop();

	if (graphics != NULL) {
		graphics->Exit();
		delete graphics;
	}

	delete keyboard;
	delete input_device;
	delete mouse;
}

void GameController::main()
{
	input(); // ����
	update(); // �X�V
	render(); // �`��
}

void GameController::input()
{
	keyboard->update();
	try
	{
		mouse->update();
	}
	catch (const char *e)
	{
#ifdef _DEBUG
		std::cout << e << std::endl;
#endif
	}
	auto mouse_current_state = mouse->getState();

	if (keyboard->getTrigger(DIK_ADD) && under_controll + 1 < airplains.size()) under_controll++;
	if (keyboard->getTrigger(DIK_SUBTRACT) && under_controll > 0) under_controll--;

	if (view_type == CameraTypes::OVER)
	{
		if (keyboard->getPress(DIK_UPARROW)) over_camera.elevation += 0.1f;
		if (keyboard->getPress(DIK_DOWNARROW)) over_camera.elevation -= 0.1f;
		if (keyboard->getPress(DIK_RIGHTARROW)) over_camera.azimuth -= 0.1f;
		if (keyboard->getPress(DIK_LEFTARROW)) over_camera.azimuth += 0.1f;
		if (keyboard->getPress(DIK_RETURN) && 0 < over_camera.distance) over_camera.distance -= 1.0f;
		if (keyboard->getPress(DIK_BACKSPACE)) over_camera.distance += 1.0f;
		over_camera.distance -= mouse_current_state.lZ / 10;
	}

	if (keyboard->getTrigger(DIK_V)) view_type++;
	if (keyboard->getTrigger(DIK_NUMPAD5)) airplains[under_controll]->switchExplosion();
	if (keyboard->getTrigger(DIK_NUMPAD8)) airplains[under_controll]->switchDrawBBox();
	if (keyboard->getTrigger(DIK_NUMPAD6)) airplains[under_controll]->addTrans({ 0, 0, -1 });
	if (keyboard->getTrigger(DIK_NUMPAD4)) airplains[under_controll]->addTrans({ 0, 0, 1 });
	if (keyboard->getTrigger(DIK_SPACE)) airplains[under_controll]->setTrans({0, 0, 0});

#ifdef _DEBUG
	printf("%ld, %ld, %ld\n", mouse_current_state.lX, mouse_current_state.lY, mouse_current_state.lZ);
#endif
}

void GameController::update()
{
	for (const auto& airplain : airplains)
	{
		airplain->update();
	}

	auto colls = getCollisions({ airplains[0]->getBBox() }, { airplains[1]->getBBox() });
	if (colls.size() > 0)
	{
#ifdef _DEBUG
		debug_text = "�������Ƃ�";
#endif // DEBUG
	}
	else
	{
#ifdef _DEBUG
		debug_text = "�������Ƃ��";
#endif // DEBUG
	}

	D3DXMATRIX mat;

	switch (view_type)
	{
	case CameraTypes::OVER:
#define OVER_CAMERA_FLAG 1
#if OVER_CAMERA_FLAG == 1
		camera->look_at = D3DXVECTOR3(0, 0, 0);
		camera->eye = D3DXVECTOR3(
			over_camera.distance * sin(over_camera.elevation) * cos(over_camera.azimuth),
			over_camera.distance * cos(over_camera.elevation),
			over_camera.distance * sin(over_camera.azimuth) * sin(over_camera.elevation)
		);
		camera->eye += camera->look_at;
		camera->up = D3DXVECTOR3(0, 1, 0);
#elif OVER_CAMERA_FLAG == 2
		camera->look_at = D3DXVECTOR3(0, 0, 0);
		camera->eye = D3DXVECTOR3(
			over_camera.distance * sin(over_camera.elevation) * cos(over_camera.azimuth),
			over_camera.distance * cos(over_camera.elevation),
			over_camera.distance * sin(over_camera.azimuth) * sin(over_camera.elevation)
		);
		D3DXMatrixInverse(&mat, NULL, &view);
		camera->up = D3DXVECTOR3(
			mat._21,
			mat._22,
			mat._23
		);
#endif
		break;
	case CameraTypes::TPS:
		camera->look_at = D3DXVECTOR3(airplains[under_controll]->getMat()._41, airplains[under_controll]->getMat()._42, airplains[under_controll]->getMat()._43);
		camera->eye = camera->look_at - 10 * D3DXVECTOR3(airplains[under_controll]->getMat()._31, airplains[under_controll]->getMat()._32, airplains[under_controll]->getMat()._33);
		camera->up = D3DXVECTOR3(airplains[under_controll]->getMat()._21, airplains[under_controll]->getMat()._22, airplains[under_controll]->getMat()._23);
		break;
	case CameraTypes::FPS:
		camera->eye = 2 * D3DXVECTOR3(airplains[under_controll]->getMat()._41, airplains[under_controll]->getMat()._42, airplains[under_controll]->getMat()._43); // ���炳�Ȃ��Ɩ{�̂Ɣ�����܂��̂�
		camera->look_at = camera->eye + 10 * D3DXVECTOR3(airplains[under_controll]->getMat()._31, airplains[under_controll]->getMat()._32, airplains[under_controll]->getMat()._33);
		camera->up = D3DXVECTOR3(airplains[under_controll]->getMat()._21, airplains[under_controll]->getMat()._22, airplains[under_controll]->getMat()._23);
		break;
	}

	D3DXMatrixLookAtLH(&view, &camera->eye, &camera->look_at, &camera->up);

	// �v���W�F�N�V�����ϊ��s��쐬
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI / 2,					// ����p
		(float)width / (float)height,	// �A�X�y�N�g��
		1.0f,							// �j�A�v���[��
		5000.0f							// �t�@�[�v���[��
	);
}

void GameController::render()
{
	graphics->SetView(view); // �J�����s����Œ�p�C�v���C���փZ�b�g
	graphics->SetProjection(proj); // �ˉe�ϊ��s����Œ�p�C�v���C���փZ�b�g
	graphics->Render([&](LPDIRECT3DDEVICE9 device) {
		for (const auto& airplain : airplains) airplain->draw(device);
		skydome->draw(device);
#ifdef _DEBUG
		debug_font->DrawTextA(0, 0, debug_text.c_str());
#endif // _DEBUG
	});
}