#ifndef _VE_REGISTRAR_
#define _VE_REGISTRAR_
#include <map>
#include <functional>

template<typename ID, class T>
class veRegistrar
{
public:

	~veRegistrar(){};

	static veRegistrar<ID, T>* instance(){
		static veRegistrar<ID, T> registrar;
		return &registrar;
	}

	bool reg(const ID id, T *t){
		auto iter = _registry.find(id);
		if (iter != _registry.end()) return false;
		_registry[id] = t;
		if (!_registerCallbacks.empty()){
			for (auto &cb : _registerCallbacks){
				cb(id, t);
			}
		}
		return true;
	}

	bool unReg(const ID id){
		auto iter = _registry.find(id);
		if (iter != _registry.end()){
			if (!_unRegisterCallbacks.empty()){
				for (auto &cb : _unRegisterCallbacks){
					cb(iter->first, iter->second);
				}
			}
			_registry.erase(iter);
			return true;
		}
		return false;
	}

	T* getRegContent(const ID id){
		auto iter = _registry.find(id);
		if (iter != _registry.end()){
			return iter->second;
		}
		return nullptr;
	}

	const std::map<ID, T*>& getRegistry() const { return _registry; }

	void addRegisterCallback(const std::function<void(ID, T*)> &func) { _registerCallbacks.push_back(func); }
	void addUnRegisterCallback(const std::function<void(ID, T*)> &func) { _unRegisterCallbacks.push_back(func); }

protected:

	veRegistrar(){};

private:

	std::map<ID, T*> _registry;
	std::vector< std::function<void(ID, T*)> > _registerCallbacks;
	std::vector< std::function<void(ID, T*)> > _unRegisterCallbacks;
};

template<typename ID, typename T, class C>
class veRegistrarAgent
{
public:
	veRegistrarAgent(const ID &id)
		: _id(id)
	{
		_c = new C();
		veRegistrar<ID, T>::instance()->reg(id, _c);
	}
	~veRegistrarAgent(){
		veRegistrar<ID, T>::instance()->unReg(_id);
		delete _c;
	}

private:
	ID _id;
	C *_c;
};

#endif