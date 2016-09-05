#include "RenderState.h"

veBlendFunc veBlendFunc::DISABLE = { GL_ONE, GL_ZERO };
veBlendFunc veBlendFunc::ADDITIVE = { GL_SRC_ALPHA, GL_ONE };
veBlendFunc veBlendFunc::ALPHA = { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA };

veStencilFunc veStencilFunc::ALWAYS = { GL_ALWAYS, 0, 0, GL_ALWAYS, 0, 0 };
veStencilOp veStencilOp::KEEP = { GL_KEEP, GL_KEEP, GL_KEEP, GL_KEEP, GL_KEEP, GL_KEEP };

veRenderState::veRenderState()
	: _isInited(false)
{

}


void veRenderState::init()
{

    glEnable(GL_DEPTH_TEST);
    _depthTest = _currentDepthTest = true;

    glDepthMask(GL_TRUE);
    _depthWrite = _currentDepthWrite = true;

	glDisable(GL_SCISSOR_TEST);
	_scissorTest = _currentScissorTest = false;

    glEnable(GL_CULL_FACE);
    _cullFace = _currentCullFace = true;
    glCullFace(GL_BACK);
    _cullfaceMode = _currentCullfaceMode = GL_BACK;

    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    glBlendEquation(GL_FUNC_ADD);
    _blendFunc = _currentBlendFunc = veBlendFunc::DISABLE;
    _blendEquation = _currentBlendEquation = GL_FUNC_ADD;

    glDisable(GL_STENCIL_TEST);
    glStencilFuncSeparate(GL_FRONT, GL_ALWAYS, 0, 0);
    glStencilFuncSeparate(GL_BACK, GL_ALWAYS, 0, 0);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
    _stencilTest = _currentStencilTest = false;
    _stencilFunc = _currentStencilFunc = veStencilFunc::ALWAYS;
    _stencilOp = _currentStencilOp = veStencilOp::KEEP;
}

veRenderState::~veRenderState()
{

}

void veRenderState::applyState()
{
	if (true) {
		if (_depthTest != _currentDepthTest) {
			_depthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
			_currentDepthTest = _depthTest;
		}

		if (_depthWrite != _currentDepthWrite) {
			_depthWrite ? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
			_currentDepthWrite = _depthWrite;
		}


		if (_scissorTest != _currentScissorTest) {
			_scissorTest ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
			_currentScissorTest = _scissorTest;
		}

		if (_cullFace != _currentCullFace) {
			_cullFace ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
			_currentCullFace = _cullFace;
		}

		if (_currentCullFace) {
			if (_cullfaceMode != _currentCullfaceMode) {
				glCullFace(_cullfaceMode);
				_currentCullfaceMode = _cullfaceMode;
			}
		}

		if (_blendFunc != _currentBlendFunc) {
			if (_blendFunc != veBlendFunc::DISABLE) {
				glEnable(GL_BLEND);
				glBlendFunc(_blendFunc.src, _blendFunc.dst);
				if (_blendEquation != _currentBlendEquation) {
					glBlendEquation(_blendEquation);
					_currentBlendEquation = _blendEquation;
				}
			}
			else {
				glDisable(GL_BLEND);
			}
			_currentBlendFunc = _blendFunc;
		}

		if (_stencilTest != _currentStencilTest) {
			_stencilTest ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
			_currentStencilTest = _stencilTest;
		}

		if (_currentStencilTest) {
			if (_stencilFunc != _currentStencilFunc) {
				glStencilFuncSeparate(GL_FRONT, _stencilFunc.frontfunc, _stencilFunc.frontref, _stencilFunc.frontmask);
				glStencilFuncSeparate(GL_BACK, _stencilFunc.backfunc, _stencilFunc.backref, _stencilFunc.backmask);
				_currentStencilFunc = _stencilFunc;
			}

			if (_stencilOp != _currentStencilOp) {
				glStencilOpSeparate(GL_FRONT, _stencilOp.frontsfail, _stencilOp.frontdpfail, _stencilOp.frontdppass);
				glStencilOpSeparate(GL_BACK, _stencilOp.backsfail, _stencilOp.backdpfail, _stencilOp.backdppass);
				_currentStencilOp = _stencilOp;
			}
		}
	}
}

void veRenderState::resetState()
{
	if (!_isInited) {
		init();
		_isInited = true;
	}else {
		if (_currentDepthTest != true) {
			glEnable(GL_DEPTH_TEST);
			_depthTest = _currentDepthTest = true;
		}

		if (_currentDepthWrite != true) {
			glDepthMask(GL_TRUE);
			_depthWrite = _currentDepthWrite = true;
		}

		if (_currentScissorTest != false) {
			glDisable(GL_SCISSOR_TEST);
			_scissorTest = _currentScissorTest = false;
		}

		if (_currentCullFace != true) {
			glEnable(GL_CULL_FACE);
			_cullFace = _currentCullFace = true;
		}

		if (_currentCullfaceMode != GL_BACK) {
			glCullFace(GL_BACK);
			_cullfaceMode = _currentCullfaceMode = GL_BACK;
		}

		if (_currentBlendFunc != veBlendFunc::DISABLE) {
			glDisable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ZERO);
		}

		if (_currentBlendEquation != GL_FUNC_ADD) {
			glBlendEquation(GL_FUNC_ADD);
			_cullfaceMode = _currentCullfaceMode = GL_BACK;
		}

		if (_currentStencilTest != false) {
			glDisable(GL_STENCIL_TEST);
			_stencilTest = _currentStencilTest = false;
		}

		if (_currentStencilFunc != veStencilFunc::ALWAYS) {
			glStencilFuncSeparate(GL_FRONT, GL_ALWAYS, 0, 0);
			glStencilFuncSeparate(GL_BACK, GL_ALWAYS, 0, 0);
			_stencilFunc = _currentStencilFunc = veStencilFunc::ALWAYS;
		}

		if (_currentStencilOp != veStencilOp::KEEP) {
			glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP);
			glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
			_stencilOp = _currentStencilOp = veStencilOp::KEEP;
		}
	}
}