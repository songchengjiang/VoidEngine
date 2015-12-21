#include "RenderState.h"

veBlendFunc veBlendFunc::DISABLE = { GL_ONE, GL_ZERO };
veBlendFunc veBlendFunc::ADDITIVE = { GL_SRC_ALPHA, GL_ONE };
veBlendFunc veBlendFunc::ALPHA = { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA };

veStencilFunc veStencilFunc::ALWAYS = { GL_ALWAYS, 0, 0, GL_ALWAYS, 0, 0 };
veStencilOp veStencilOp::KEEP = { GL_KEEP, GL_KEEP, GL_KEEP, GL_KEEP, GL_KEEP, GL_KEEP };

static bool          DEPTH_TEST = false;
static bool          DEPTH_WRITE = false;
static bool          STENCIL_TEST = true;
static bool          CULL_FACE = false;
static GLenum        CULL_FACE_MODE;
static veBlendFunc   BLEND_FUNC;
static GLenum        BLEND_EQUATION;
static veStencilFunc STENCIL_FUNC;
static veStencilOp   STENCIL_OP;

veRenderState::veRenderState()
	: _isInited(false)
	, _isLockState(false)
{

}


void veRenderState::init()
{
	if (!DEPTH_TEST) {
		glEnable(GL_DEPTH_TEST);
		_depthTest = DEPTH_TEST = true;
	}

	if (!DEPTH_WRITE) {
		glDepthMask(GL_TRUE);
		_depthWrite = DEPTH_WRITE = true;
	}

	if (!CULL_FACE) {
		glEnable(GL_CULL_FACE);
		_cullFace = CULL_FACE = true;
		glCullFace(GL_BACK);
		_cullfaceMode = CULL_FACE_MODE = GL_BACK;
	}

	if (BLEND_FUNC != veBlendFunc::DISABLE) {
		glDisable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ZERO);
		glBlendEquation(GL_FUNC_ADD);
		_blendFunc = BLEND_FUNC = veBlendFunc::DISABLE;
		_blendEquation = BLEND_EQUATION = GL_FUNC_ADD;
	}

	if (STENCIL_TEST) {
		glDisable(GL_STENCIL_TEST);
		glStencilFuncSeparate(GL_FRONT, GL_ALWAYS, 0, 0);
		glStencilFuncSeparate(GL_BACK, GL_ALWAYS, 0, 0);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
		_stencilTest = STENCIL_TEST = false;
		_stencilFunc = STENCIL_FUNC = veStencilFunc::ALWAYS;
		_stencilOp = STENCIL_OP = veStencilOp::KEEP;
	}
}

veRenderState::~veRenderState()
{

}

veRenderState* veRenderState::instance()
{
	static veRenderState rs;
	return &rs;
}

void veRenderState::applyState()
{
	if (!_isInited) {
		init();
		_isInited = true;
	}
	if (!_isLockState) {
		if (_depthTest != DEPTH_TEST) {
			_depthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
			DEPTH_TEST = _depthTest;
		}

		if (_depthWrite != DEPTH_WRITE) {
			_depthWrite ? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
			DEPTH_WRITE = _depthWrite;
		}

		if (_cullFace != CULL_FACE) {
			_cullFace ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
			CULL_FACE = _cullFace;
		}

		if (CULL_FACE) {
			if (_cullfaceMode != CULL_FACE_MODE) {
				glCullFace(_cullfaceMode);
				CULL_FACE_MODE = _cullfaceMode;
			}
		}

		if (_blendFunc != BLEND_FUNC) {
			if (_blendFunc != veBlendFunc::DISABLE) {
				glEnable(GL_BLEND);
				glBlendFunc(_blendFunc.src, _blendFunc.dst);
				if (_blendEquation != BLEND_EQUATION) {
					glBlendEquation(_blendEquation);
					BLEND_EQUATION = _blendEquation;
				}
			}
			else {
				glDisable(GL_BLEND);
			}
			BLEND_FUNC = _blendFunc;
		}

		if (_stencilTest != STENCIL_TEST) {
			_stencilTest ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
			STENCIL_TEST = _stencilTest;
		}

		if (STENCIL_TEST) {
			if (_stencilFunc != STENCIL_FUNC) {
				glStencilFuncSeparate(GL_FRONT, _stencilFunc.frontfunc, _stencilFunc.frontref, _stencilFunc.frontmask);
				glStencilFuncSeparate(GL_BACK, _stencilFunc.backfunc, _stencilFunc.backref, _stencilFunc.backmask);
				STENCIL_FUNC = _stencilFunc;
			}

			if (_stencilOp != STENCIL_OP) {
				glStencilOpSeparate(GL_FRONT, _stencilOp.frontsfail, _stencilOp.frontdpfail, _stencilOp.frontdppass);
				glStencilOpSeparate(GL_BACK, _stencilOp.backsfail, _stencilOp.backdpfail, _stencilOp.backdppass);
				STENCIL_OP = _stencilOp;
			}
		}
	}
}

void veRenderState::resetState()
{
	init();
}