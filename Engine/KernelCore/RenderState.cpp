#include "RenderState.h"

veBlendFunc veBlendFunc::DISABLE = { GL_ONE, GL_ZERO };
veBlendFunc veBlendFunc::ADDITIVE = { GL_SRC_ALPHA, GL_ONE };
veBlendFunc veBlendFunc::ALPHA = { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA };

veStencilFunc veStencilFunc::ALWAYS = { GL_ALWAYS, 0, 0, GL_ALWAYS, 0, 0 };
veStencilOp veStencilOp::KEEP = { GL_KEEP, GL_KEEP, GL_KEEP, GL_KEEP, GL_KEEP, GL_KEEP };

veRenderState::veRenderState()
	: _isInited(false)
    , _defaultRS(nullptr)
{
}

veRenderState::~veRenderState()
{
    if (_defaultRS) {
        delete _defaultRS;
    }
}

void veRenderState::applyState()
{
    if (_depthTest != _defaultRS->_depthTest) {
        _depthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
        _defaultRS->_depthTest = _depthTest;
    }
    
    if (_depthWrite != _defaultRS->_depthWrite) {
        _depthWrite ? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
        _defaultRS->_depthWrite = _depthWrite;
    }
    
    
    if (_scissorTest != _defaultRS->_scissorTest) {
        _scissorTest ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
        _defaultRS->_scissorTest = _scissorTest;
    }
    
    if (_cullFace != _defaultRS->_cullFace) {
        _cullFace ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
        _defaultRS->_cullFace = _cullFace;
    }
    
    if (_cullFace) {
        if (_cullfaceMode != _defaultRS->_cullfaceMode) {
            glCullFace(_cullfaceMode);
            _defaultRS->_cullfaceMode = _cullfaceMode;
        }
    }
    
    if (_blendFunc != _defaultRS->_blendFunc) {
        if (_blendFunc != veBlendFunc::DISABLE) {
            glEnable(GL_BLEND);
            glBlendFunc(_blendFunc.src, _blendFunc.dst);
            if (_blendEquation != _defaultRS->_blendEquation) {
                glBlendEquation(_blendEquation);
                _defaultRS->_blendEquation = _blendEquation;
            }
        }
        else {
            glDisable(GL_BLEND);
        }
        _defaultRS->_blendFunc = _blendFunc;
    }
    
    if (_stencilTest != _defaultRS->_stencilTest) {
        _stencilTest ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
        _defaultRS->_stencilTest = _stencilTest;
    }
    
    if (_stencilTest) {
        if (_stencilFunc != _defaultRS->_stencilFunc) {
            glStencilFuncSeparate(GL_FRONT, _stencilFunc.frontfunc, _stencilFunc.frontref, _stencilFunc.frontmask);
            glStencilFuncSeparate(GL_BACK, _stencilFunc.backfunc, _stencilFunc.backref, _stencilFunc.backmask);
            _defaultRS->_stencilFunc = _stencilFunc;
        }
        
        if (_stencilOp != _defaultRS->_stencilOp) {
            glStencilOpSeparate(GL_FRONT, _stencilOp.frontsfail, _stencilOp.frontdpfail, _stencilOp.frontdppass);
            glStencilOpSeparate(GL_BACK, _stencilOp.backsfail, _stencilOp.backdpfail, _stencilOp.backdppass);
            _defaultRS->_stencilOp = _stencilOp;
        }
    }
}

void veRenderState::resetState()
{
    if (!_defaultRS) {
        _defaultRS = new veRenderState();
    }
    if (!_isInited || _defaultRS->_depthTest != true) {
        glEnable(GL_DEPTH_TEST);
        _defaultRS->_depthTest = true;
    }
    
    if (!_isInited || _defaultRS->_depthWrite != true) {
        glDepthMask(GL_TRUE);
        _defaultRS->_depthWrite = true;
    }
    
    if (!_isInited || _defaultRS->_scissorTest != false) {
        glDisable(GL_SCISSOR_TEST);
        _defaultRS->_scissorTest = false;
    }
    
    if (!_isInited || _defaultRS->_cullFace != false) {
        glEnable(GL_CULL_FACE);
        _defaultRS->_cullFace = true;
    }
    
    if (!_isInited || _defaultRS->_cullfaceMode != false) {
        glCullFace(GL_BACK);
        _defaultRS->_cullfaceMode = GL_BACK;
    }
    
    if (!_isInited || _defaultRS->_blendFunc != veBlendFunc::DISABLE) {
        glDisable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ZERO);
        _defaultRS->_blendFunc = veBlendFunc::DISABLE;
    }
    
    if (!_isInited || _defaultRS->_blendEquation != GL_FUNC_ADD) {
        glBlendEquation(GL_FUNC_ADD);
        _defaultRS->_blendEquation = GL_FUNC_ADD;
    }
    
    if (!_isInited || _defaultRS->_stencilTest != false) {
        glDisable(GL_STENCIL_TEST);
        _defaultRS->_stencilTest = false;
    }
    
    if (!_isInited || _defaultRS->_stencilFunc != veStencilFunc::ALWAYS) {
        glStencilFuncSeparate(GL_FRONT, GL_ALWAYS, 0, 0);
        glStencilFuncSeparate(GL_BACK, GL_ALWAYS, 0, 0);
        _defaultRS->_stencilFunc = veStencilFunc::ALWAYS;
    }

    if (!_isInited || _defaultRS->_stencilOp != veStencilOp::KEEP) {
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
        _defaultRS->_stencilOp = veStencilOp::KEEP;
    }
    
    _isInited = true;
}
