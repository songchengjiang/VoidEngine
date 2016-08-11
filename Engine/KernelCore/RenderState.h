#ifndef _VE_RENDER_STATE_
#define _VE_RENDER_STATE_
#include "Prerequisites.h"

struct VE_EXPORT veBlendFunc
{
	static veBlendFunc DISABLE;
	static veBlendFunc ADDITIVE;
	static veBlendFunc ALPHA;

	GLenum src;
	GLenum dst;

	inline bool operator == (const veBlendFunc& rkbf) const {
		return (src == rkbf.src && dst == rkbf.dst);
	}

	inline bool operator != (const veBlendFunc& rkbf) const {
		return (src != rkbf.src || dst != rkbf.dst);
	}
};

struct VE_EXPORT veStencilFunc
{
	static veStencilFunc ALWAYS;

	GLenum frontfunc;
	GLint  frontref;
	GLuint frontmask;

	GLenum backfunc;
	GLint  backref;
	GLuint backmask;

	inline bool operator == (const veStencilFunc& rkbf) const {
		return (frontfunc == rkbf.frontfunc && frontref == rkbf.frontref && frontmask == rkbf.frontmask
			&& backfunc == rkbf.backfunc && backref == rkbf.backref && backmask == rkbf.backmask);
	}

	inline bool operator != (const veStencilFunc& rkbf) const {
		return (frontfunc != rkbf.frontfunc || frontref != rkbf.frontref || frontmask != rkbf.frontmask
			|| backfunc != rkbf.backfunc || backref != rkbf.backref || backmask != rkbf.backmask);
	}
};

struct VE_EXPORT veStencilOp
{
	static veStencilOp KEEP;

	GLenum frontsfail;
	GLenum frontdpfail;
	GLenum frontdppass;

	GLenum backsfail;
	GLenum backdpfail;
	GLenum backdppass;

	inline bool operator == (const veStencilOp& rkbf) const {
		return (frontsfail == rkbf.frontsfail && frontdpfail == rkbf.frontdpfail && frontdppass == rkbf.frontdppass
			&& backsfail == rkbf.backsfail && backdpfail == rkbf.backdpfail && backdppass == rkbf.backdppass);
	}

	inline bool operator != (const veStencilOp& rkbf) const {
		return (frontsfail != rkbf.frontsfail || frontdpfail != rkbf.frontdpfail || frontdppass != rkbf.frontdppass
			|| backsfail != rkbf.backsfail || backdpfail != rkbf.backdpfail || backdppass != rkbf.backdppass);
	}
};

class VE_EXPORT veRenderState
{
public:
    veRenderState();
	~veRenderState();

	void applyState();
	void resetState();

	void setDepthTest(bool isTrue) { _depthTest = isTrue; };
	void setDepthWrite(bool isTrue) { _depthWrite = isTrue; }
	void setStencilTest(bool isTrue) { _stencilTest = isTrue; }
	void setCullface(bool isTrue) { _cullFace = isTrue; }
	void setCullfaceMode(GLenum mode) { _cullfaceMode = mode; }
	void setBlendFunc(const veBlendFunc &func) { _blendFunc = func; }
	void setBlendEquation(GLenum equation) { _blendEquation = equation; }
	void setStencilFunc(const veStencilFunc &func) { _stencilFunc = func; }
	void setStencilOp(const veStencilOp &op) { _stencilOp = op; }

private:
	void init();

private:

	bool          _depthTest;
	bool          _depthWrite;
	bool          _stencilTest;
	bool          _cullFace;
	GLenum        _cullfaceMode;
	veBlendFunc   _blendFunc;
	GLenum        _blendEquation;
	veStencilFunc _stencilFunc;
	veStencilOp   _stencilOp;
    
    bool          _currentDepthTest;
    bool          _currentDepthWrite;
    bool          _currentStencilTest;
    bool          _currentCullFace;
    GLenum        _currentCullfaceMode;
    veBlendFunc   _currentBlendFunc;
    GLenum        _currentBlendEquation;
    veStencilFunc _currentStencilFunc;
    veStencilOp   _currentStencilOp;
    
	bool _isInited;
};

#endif