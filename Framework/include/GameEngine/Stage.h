#pragma once

enum class Stage {
	PreInit = 0,
	Init,
	PostInit,
	PreRender,
	Render,
	PostRender,
	Update,
};