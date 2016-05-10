#include "FileReaderWriter.h"
#include <rapidjson/include/document.h>
#include "Constants.h"
#include "KernelCore/Material.h"
#include "KernelCore/SceneManager.h"
#include "BaseCore/Value.h"

#include "EffectCore/ParticleSystem.h"

#include "EffectCore/QuatRenderer.h"

#include "EffectCore/PointEmitter.h"
#include "EffectCore/BoxEmitter.h"
#include "EffectCore/CircleEmitter.h"
#include "EffectCore/LineEmitter.h"
#include "EffectCore/SphereEmitter.h"

#include "EffectCore/ColorAffector.h"
#include "EffectCore/ScaleAffector.h"
#include "EffectCore/ScaleVelocityAffector.h"
#include "EffectCore/GravityAffector.h"
#include "EffectCore/JetAffector.h"
#include "EffectCore/LinearForceAffector.h"
#include "EffectCore/SineForceAffector.h"
#include "EffectCore/VortexAffector.h"
#include "EffectCore/FlockCenteringAffector.h"

#include <unordered_map>

using namespace rapidjson;
class veFileReaderWriterPARTICLE : public veFileReaderWriter
{
public:
	veFileReaderWriterPARTICLE()
		: _doucument(nullptr)
        , _particleSystem(nullptr)
	{};
	~veFileReaderWriterPARTICLE(){};

	virtual void* readFile(veSceneManager *sm, const std::string &filePath, const std::string &name, const veFileParam &param) override{
		_sceneManager = sm;
		_name = name;
		_doucument = new Document;
		auto fileData = veFile::instance()->readFileToBuffer(filePath);
		_doucument->Parse(fileData->buffer);
		if (_doucument->HasParseError()) return nullptr;
        readParticleSystem();
		VE_SAFE_DELETE(_doucument);
		return _particleSystem;
	}

	virtual bool writeFile(veSceneManager *sm, void *data, const std::string &filePath) override{
		return true;
	}

private:
    
    void readParticleSystem(){
        _particleSystem = _sceneManager->createParticleSystem(_name);
        if ((*_doucument).HasMember(MATERIALS_KEY.c_str())) {
            const Value &val = (*_doucument)[MATERIALS_KEY.c_str()];
            auto materials = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, val.GetString(), _name + std::string("-materials")));
            _particleSystem->setMaterialArray(materials);
        }
        
        if ((*_doucument).HasMember(RENDERER_KEY.c_str())) {
            const Value &val = (*_doucument)[RENDERER_KEY.c_str()];
            readRenderer(val);
        }
        
        if ((*_doucument).HasMember(EMITTERS_KEY.c_str())){
            const Value &val = (*_doucument)[EMITTERS_KEY.c_str()];
            for (unsigned int i = 0; i < val.Size(); ++i){
                readEmitter(val[i]);
            }
        }
        
        if ((*_doucument).HasMember(AFFECTORS_KEY.c_str())){
            const Value &val = (*_doucument)[AFFECTORS_KEY.c_str()];
            for (unsigned int i = 0; i < val.Size(); ++i){
                readAffector(val[i]);
            }
        }
    }
    
    void readRenderer(const Value &rendererVal){
        
        if (rendererVal.HasMember(TYPE_KEY.c_str())){
            std::string type = rendererVal[TYPE_KEY.c_str()].GetString();
            veRenderer *renderer = nullptr;
            if (type == RENDERER_QUAT_KEY){
                renderer = new veQuatRenderer;
            }
            _particleSystem->setRenderer(renderer);
        }
    }
    
    void readEmitter(const Value &emitterVal){
        veEmitter *emitter = nullptr;
        if (emitterVal.HasMember(TYPE_KEY.c_str())){
            std::string type = emitterVal[TYPE_KEY.c_str()].GetString();
            if (type == EMITTER_BOX_KEY){
                emitter = new veBoxEmitter;
                readBoxEmitter(emitter, emitterVal);
            }else if (type == EMITTER_LINE_KEY){
                emitter = new veLineEmitter;
                readLineEmitter(emitter, emitterVal);
            }else if (type == EMITTER_CIRCLE_KEY){
                emitter = new veCircleEmitter;
                readCircleEmitter(emitter, emitterVal);
            } else if (type == EMITTER_SPHERE_KEY){
                emitter = new veSphereEmitter;
                readSphereEmitter(emitter, emitterVal);
            }else {
                emitter = new vePointEmitter;
            }
        }
        
        if (emitterVal.HasMember(NAME_KEY.c_str())){
            emitter->setName(emitterVal[NAME_KEY.c_str()].GetString());
        }
        
        if (emitterVal.HasMember(KEEP_LOCAL_KEY.c_str())){
            emitter->keepLocalParticles(emitterVal[KEEP_LOCAL_KEY.c_str()].GetBool());
        }
        
        if (emitterVal.HasMember(IS_ENABLED_KEY.c_str())){
            emitter->setEnabled(emitterVal[IS_ENABLED_KEY.c_str()].GetBool());
        }
        
        if (emitterVal.HasMember(COLOR_KEY.c_str())){
            const Value &val = emitterVal[COLOR_KEY.c_str()];
            if (val.Size() == 4){
                emitter->setParticleColor(veVec4(val[0].GetDouble(), val[1].GetDouble(), val[2].GetDouble(), val[3].GetDouble()));
            }
        }
        
        if (emitterVal.HasMember(POSITION_KEY.c_str())){
            const Value &val = emitterVal[POSITION_KEY.c_str()];
            if (val.Size() == 3){
                emitter->setPosition(veVec3(val[0].GetDouble(), val[1].GetDouble(), val[2].GetDouble()));
            }
        }
        
        if (emitterVal.HasMember(ROTATION_KEY.c_str())){
            const Value &val = emitterVal[ROTATION_KEY.c_str()];
            if (val.Size() == 4){
                emitter->setRotation(veQuat(val[0].GetDouble(), val[1].GetDouble(), val[2].GetDouble(), val[3].GetDouble()));
            }
        }
        
        if (emitterVal.HasMember(ORIENTATION_KEY.c_str())){
            const Value &val = emitterVal[ANGLE_KEY.c_str()];
            if (val.Size() == 2){
                veQuat start, end;
                const Value &startVal = val[START_KEY.c_str()];
                const Value &endVal   = val[END_KEY.c_str()];
                start = veQuat(startVal[0].GetDouble(), startVal[1].GetDouble(), startVal[2].GetDouble(), startVal[3].GetDouble());
                end   = veQuat(endVal[0].GetDouble(), endVal[1].GetDouble(), endVal[2].GetDouble(), endVal[3].GetDouble());
                emitter->setOrientation(start, end);
            }
        }
        
        if (emitterVal.HasMember(ANGLE_KEY.c_str())){
            const Value &val = emitterVal[ANGLE_KEY.c_str()];
            emitter->setAngle(getValue(val));
        }
        
        if (emitterVal.HasMember(EMISSION_RATE_KEY.c_str())){
            const Value &val = emitterVal[EMISSION_RATE_KEY.c_str()];
            emitter->setEmissionRate(getValue(val));
        }
        
        if (emitterVal.HasMember(TOTAL_TIME_TO_LIVE_KEY.c_str())){
            const Value &val = emitterVal[TOTAL_TIME_TO_LIVE_KEY.c_str()];
            emitter->setTotalTimeToLive(getValue(val));
        }
        
        if (emitterVal.HasMember(MASS_KEY.c_str())){
            const Value &val = emitterVal[MASS_KEY.c_str()];
            emitter->setMass(getValue(val));
        }
        
        if (emitterVal.HasMember(VELOCITY_KEY.c_str())){
            const Value &val = emitterVal[VELOCITY_KEY.c_str()];
            emitter->setVelocity(getValue(val));
        }
        
        if (emitterVal.HasMember(DURATION_KEY.c_str())){
            const Value &val = emitterVal[DURATION_KEY.c_str()];
            emitter->setDuration(getValue(val));
        }
        
        if (emitterVal.HasMember(REPEAT_DELAY_KEY.c_str())){
            const Value &val = emitterVal[ANGLE_KEY.c_str()];
            emitter->setRepeatDelay(getValue(val));
        }
    
        if (emitterVal.HasMember(WIDTH_KEY.c_str())){
            const Value &val = emitterVal[WIDTH_KEY.c_str()];
            emitter->setParticleWidth(getValue(val));
        }
        
        if (emitterVal.HasMember(HEIGHT_KEY.c_str())){
            const Value &val = emitterVal[HEIGHT_KEY.c_str()];
            emitter->setParticleHeight(getValue(val));
        }
        
        if (emitterVal.HasMember(DEPTH_KEY.c_str())){
            const Value &val = emitterVal[DEPTH_KEY.c_str()];
            emitter->setParticleDepth(getValue(val));
        }
        
        _particleSystem->addEmitter(emitter);
    }
    
    void readBoxEmitter(veEmitter *emitter, const Value &emitterVal){
        auto boxEmitter = static_cast<veBoxEmitter *>(emitter);
        if (emitterVal.HasMember(BOX_EMITTER_WIDTH_KEY.c_str())){
            const Value &val = emitterVal[BOX_EMITTER_WIDTH_KEY.c_str()];
            boxEmitter->setBoxWidth(val.GetDouble());
        }
        if (emitterVal.HasMember(BOX_EMITTER_HEIGHT_KEY.c_str())){
            const Value &val = emitterVal[BOX_EMITTER_HEIGHT_KEY.c_str()];
            boxEmitter->setBoxHeight(val.GetDouble());
        }
        if (emitterVal.HasMember(BOX_EMITTER_DEPTH_KEY.c_str())){
            const Value &val = emitterVal[BOX_EMITTER_DEPTH_KEY.c_str()];
            boxEmitter->setBoxDepth(val.GetDouble());
        }
    }
    
    void readLineEmitter(veEmitter *emitter, const Value &emitterVal){
        auto lineEmitter = static_cast<veLineEmitter *>(emitter);
        if (emitterVal.HasMember(LINE_EMITTER_START_KEY.c_str())){
            const Value &val = emitterVal[LINE_EMITTER_START_KEY.c_str()];
            if (val.Size() == 3){
                lineEmitter->setStart(veVec3(val[0].GetDouble(), val[1].GetDouble(), val[2].GetDouble()));
            }
        }
        
        if (emitterVal.HasMember(LINE_EMITTER_END_KEY.c_str())){
            const Value &val = emitterVal[LINE_EMITTER_END_KEY.c_str()];
            if (val.Size() == 3){
                lineEmitter->setEnd(veVec3(val[0].GetDouble(), val[1].GetDouble(), val[2].GetDouble()));
            }
        }
        
        if (emitterVal.HasMember(LINE_EMITTER_SEGMENTS_KEY.c_str())){
            const Value &val = emitterVal[LINE_EMITTER_SEGMENTS_KEY.c_str()];
            lineEmitter->setLineSegments(val.GetUint());
        }
        
        if (emitterVal.HasMember(LINE_EMITTER_RANDOM_RANGE_KEY.c_str())){
            const Value &val = emitterVal[LINE_EMITTER_RANDOM_RANGE_KEY.c_str()];
            lineEmitter->setUseRandomRange(val.GetBool());
        }
    }
    
    void readCircleEmitter(veEmitter *emitter, const Value &emitterVal){
        auto circleEmitter = static_cast<veCircleEmitter *>(emitter);
        if (emitterVal.HasMember(CIRCLE_EMITTER_RADIUS_KEY.c_str())){
            const Value &val = emitterVal[CIRCLE_EMITTER_RADIUS_KEY.c_str()];
            circleEmitter->setRadius(val.GetDouble());
        }
        
        if (emitterVal.HasMember(CIRCLE_EMITTER_STEP_ANGLE_KEY.c_str())){
            const Value &val = emitterVal[CIRCLE_EMITTER_STEP_ANGLE_KEY.c_str()];
            circleEmitter->setStepAngle(val.GetDouble());
        }
        
        if (emitterVal.HasMember(CIRCLE_EMITTER_RANDOM_ANGLE_KEY.c_str())){
            const Value &val = emitterVal[CIRCLE_EMITTER_RANDOM_ANGLE_KEY.c_str()];
            circleEmitter->setUseRandomAngle(val.GetBool());
        }
    }
    
    void readSphereEmitter(veEmitter *emitter, const Value &emitterVal){
        auto sphereEmitter = static_cast<veSphereEmitter *>(emitter);
        if (emitterVal.HasMember(SPHERE_EMITTER_RADIUS_KEY.c_str())){
            const Value &val = emitterVal[SPHERE_EMITTER_RADIUS_KEY.c_str()];
            sphereEmitter->setRadius(val.GetDouble());
        }
    }
    
    void readAffector(const Value &affectorVal){
        veAffector *affector = nullptr;
        
        if (affectorVal.HasMember(TYPE_KEY.c_str())){
            std::string type = affectorVal[TYPE_KEY.c_str()].GetString();
            if (type == AFFECTOR_COLOR_KEY){
                affector = new veColorAffector;
                readColorAffector(affector, affectorVal);
            }else if (type == AFFECTOR_SCALE_KEY){
                affector = new veScaleAffector;
                readScaleAffector(affector, affectorVal);
            }else if (type == AFFECTOR_SCALE_VELOCITY_KEY){
                affector = new veScaleVelocityAffector;
                readScaleVelocityAffector(affector, affectorVal);
            } else if (type == AFFECTOR_GRAVITY_KEY.c_str()){
                affector = new veGravityAffector;
                readGravityAffector(affector, affectorVal);
            } else if (type == AFFECTOR_JET_KEY.c_str()){
                affector = new veJetAffector;
                readJetAffector(affector, affectorVal);
            } else if (type == AFFECTOR_LINEAR_FORCE_KEY.c_str()){
                affector = new veLinearForceAffector;
                readLinearForceAffector(affector, affectorVal);
            } else if (type == AFFECTOR_SINE_FORCE_KEY.c_str()){
                affector = new veSineForceAffector;
                readSineForceAffector(affector, affectorVal);
            } else if (type == AFFECTOR_VORTEX_KEY.c_str()){
                affector = new veVortexAffector;
                readVortexAffector(affector, affectorVal);
            } else if (type == AFFECTOR_FLOCK_CENTERING_KEY.c_str()){
                affector = new veFlockCenteringAffector;
            }
        }
        
        if (!affector) return;
        
        if (affectorVal.HasMember(IS_ENABLED_KEY.c_str())){
            affector->setEnabled(affectorVal[IS_ENABLED_KEY.c_str()].GetBool());
        }
        
        if (affectorVal.HasMember(POSITION_KEY.c_str())){
            const Value &val = affectorVal[POSITION_KEY.c_str()];
            if (val.Size() == 3){
                affector->setPosition(veVec3(val[0].GetDouble(), val[1].GetDouble(), val[2].GetDouble()));
            }
        }
        
        if (affectorVal.HasMember(ROTATION_KEY.c_str())){
            const Value &val = affectorVal[ROTATION_KEY.c_str()];
            if (val.Size() == 4){
                affector->setRotation(veQuat(val[0].GetDouble(), val[1].GetDouble(), val[2].GetDouble(), val[3].GetDouble()));
            }
        }
        
        _particleSystem->addAffector(affector);
    }
    
    void readColorAffector(veAffector *affector, const Value &affectorVal){
        auto colorAffector = static_cast<veColorAffector *>(affector);
        if (affectorVal.HasMember(COLOR_AFFECTOR_COLORS_KEY.c_str())){
            const Value &val = affectorVal[COLOR_AFFECTOR_COLORS_KEY.c_str()];
            for (unsigned int i = 0; i < val.Size(); ++i){
                const Value &cols = val[i];
                if (cols.Size() == 5){
                    colorAffector->addColor(cols[0].GetDouble(), veVec4(cols[1].GetDouble(), cols[2].GetDouble(), cols[3].GetDouble(), cols[4].GetDouble()));
                }
            }
        }
        
        if (affectorVal.HasMember(COLOR_AFFECTOR_COLOR_OP_KEY.c_str())){
            std::string colOp = affectorVal[COLOR_AFFECTOR_COLOR_OP_KEY.c_str()].GetString();
            if (colOp == COLOR_AFFECTOR_COLOR_SET_KEY){
                colorAffector->setColorOperation(veColorAffector::ColorOperation::CAO_SET);
            }else {
                colorAffector->setColorOperation(veColorAffector::ColorOperation::CAO_MULTIPLY);
            }
        }
    }
    
    void readScaleAffector(veAffector *affector, const Value &affectorVal){
        auto scaleAffector = static_cast<veScaleAffector *>(affector);
        if (affectorVal.HasMember(SCALE_AFFECTOR_SCALE_X_KEY.c_str())){
            const Value &val = affectorVal[SCALE_AFFECTOR_SCALE_X_KEY.c_str()];
            scaleAffector->setScaleX(getValue(val));
        }
        
        if (affectorVal.HasMember(SCALE_AFFECTOR_SCALE_Y_KEY.c_str())){
            const Value &val = affectorVal[SCALE_AFFECTOR_SCALE_Y_KEY.c_str()];
            scaleAffector->setScaleY(getValue(val));
        }
        
        if (affectorVal.HasMember(SCALE_AFFECTOR_SCALE_Z_KEY.c_str())){
            const Value &val = affectorVal[SCALE_AFFECTOR_SCALE_Z_KEY.c_str()];
            scaleAffector->setScaleZ(getValue(val));
        }
    }
    
    void readScaleVelocityAffector(veAffector *affector, const Value &affectorVal){
        auto scaleVelAffector = static_cast<veScaleVelocityAffector *>(affector);
        if (affectorVal.HasMember(SCALE_VEL_AFFECTOR_SCALE_KEY.c_str())){
            const Value &val = affectorVal[SCALE_VEL_AFFECTOR_SCALE_KEY.c_str()];
            scaleVelAffector->setScaleVelocity(getValue(val));
        }
    }
    
    void readGravityAffector(veAffector *affector, const Value &affectorVal){
        auto gravityAffector = static_cast<veGravityAffector *>(affector);
        if (affectorVal.HasMember(GRAVITY_AFFECTOR_GRAVITY_KEY.c_str())){
            const Value &val = affectorVal[GRAVITY_AFFECTOR_GRAVITY_KEY.c_str()];
            gravityAffector->setGravity(val.GetDouble());
        }
    }
    
    void readJetAffector(veAffector *affector, const Value &affectorVal){
        auto jetAffector = static_cast<veJetAffector *>(affector);
        if (affectorVal.HasMember(JET_AFFECTOR_ACCELERATION_KEY.c_str())){
            const Value &val = affectorVal[JET_AFFECTOR_ACCELERATION_KEY.c_str()];
            jetAffector->setAcceleration(getValue(val));
        }
    }
    
    void readLinearForceAffector(veAffector *affector, const Value &affectorVal){
        auto linearForceAffector = static_cast<veLinearForceAffector *>(affector);
        if (affectorVal.HasMember(BASE_FORCE_FORCE_SCALAR_KEY.c_str())){
            const Value &val = affectorVal[BASE_FORCE_FORCE_SCALAR_KEY.c_str()];
            linearForceAffector->setForceScalar(val.GetDouble());
        }
        
        if (affectorVal.HasMember(BASE_FORCE_APPLICATION_KEY.c_str())){
            std::string app = affectorVal[BASE_FORCE_APPLICATION_KEY.c_str()].GetString();
            if (app == BASE_FORCE_APPLICATION_AVERAGE_KEY){
                linearForceAffector->setForceApplication(veBaseForceAffector::ForceApplication::FA_AVERAGE);
            }else{
                linearForceAffector->setForceApplication(veBaseForceAffector::ForceApplication::FA_ADD);
            }
        }
    }
    
    void readSineForceAffector(veAffector *affector, const Value &affectorVal){
        auto sineForceAffector = static_cast<veSineForceAffector *>(affector);
        if (affectorVal.HasMember(BASE_FORCE_FORCE_SCALAR_KEY.c_str())){
            const Value &val = affectorVal[BASE_FORCE_FORCE_SCALAR_KEY.c_str()];
            sineForceAffector->setForceScalar(val.GetDouble());
        }
        
        if (affectorVal.HasMember(BASE_FORCE_APPLICATION_KEY.c_str())){
            std::string app = affectorVal[BASE_FORCE_APPLICATION_KEY.c_str()].GetString();
            if (app == BASE_FORCE_APPLICATION_AVERAGE_KEY){
                sineForceAffector->setForceApplication(veBaseForceAffector::ForceApplication::FA_AVERAGE);
            }else{
                sineForceAffector->setForceApplication(veBaseForceAffector::ForceApplication::FA_ADD);
            }
        }
        
        if (affectorVal.HasMember(SINE_FORCE_FREQUENCY_MIN_KEY.c_str())){
            const Value &val = affectorVal[SINE_FORCE_FREQUENCY_MIN_KEY.c_str()];
            sineForceAffector->setFrequencyMinimum(val.GetDouble());
        }
        
        if (affectorVal.HasMember(SINE_FORCE_FREQUENCY_MAX_KEY.c_str())){
            const Value &val = affectorVal[SINE_FORCE_FREQUENCY_MAX_KEY.c_str()];
            sineForceAffector->setFrequencyMaximun(val.GetDouble());
        }
    }
    
    void readVortexAffector(veAffector *affector, const Value &affectorVal){
        auto vortexAffector = static_cast<veVortexAffector *>(affector);
        if (affectorVal.HasMember(VORTEX_AFFECTOR_ROTATION_SPEED_KEY.c_str())){
            const Value &val = affectorVal[VORTEX_AFFECTOR_ROTATION_SPEED_KEY.c_str()];
            vortexAffector->setRotationSpeed(getValue(val));
        }
    }
    
    veValue* getValue(const Value &val){
        veValue *value = nullptr;
        if (val.HasMember(TYPE_KEY.c_str())){
            std::string type = val[TYPE_KEY.c_str()].GetString();
            if (type == FIXED_KEY){
                value = new veValueFixed;
                if (val.HasMember(VALUE_KEY.c_str())){
                    static_cast<veValueFixed *>(value)->setValue(val[VALUE_KEY.c_str()].GetDouble());
                }
            }else if (type == RANDOM_KEY){
                value = new veValueRandom;
                if (val.HasMember(MINIMUM_KEY.c_str())){
                    static_cast<veValueRandom *>(value)->setMinimum(val[MINIMUM_KEY.c_str()].GetDouble());
                }
                
                if (val.HasMember(MAXIMUM_KEY.c_str())){
                    static_cast<veValueRandom *>(value)->setMaximum(val[MAXIMUM_KEY.c_str()].GetDouble());
                }
            
            }else if (type == CURVED_KEY){
                value = new veValueCurved;
                if (val.HasMember(INTER_TYPE_KEY.c_str())){
                    std::string IT = val[INTER_TYPE_KEY.c_str()].GetString();
                    if (IT == LINEAR_KEY){
                        static_cast<veValueCurved *>(value)->setInterpolationType(veValueCurved::InterpolationType::IT_LINEAR);
                    }else {
                        static_cast<veValueCurved *>(value)->setInterpolationType(veValueCurved::InterpolationType::IT_SPLINE);
                    }
                }
                
                if (val.HasMember(CONTROL_POINTS_KEY.c_str())){
                    const Value &cpListVal = val[CONTROL_POINTS_KEY.c_str()];
                    for (unsigned int i = 0; i < cpListVal.Size(); ++i){
                        const Value &cpVal = cpListVal[i];
                        static_cast<veValueCurved *>(value)->addControlPoint(cpVal[0].GetDouble(), cpVal[1].GetDouble());
                    }
                }
                
            }else if (type == OSCILLATE_KEY){
                value = new veValueOscillate;
                if (val.HasMember(OSCILL_TYPE_KEY.c_str())){
                    std::string OT = val[OSCILL_TYPE_KEY.c_str()].GetString();
                    if (OT == OSCILL_SINE_KEY){
                        static_cast<veValueOscillate *>(value)->setOscillationType(veValueOscillate::OscillationType::OSCT_SINE);
                    }else {
                        static_cast<veValueOscillate *>(value)->setOscillationType(veValueOscillate::OscillationType::OSCT_SQUARE);
                    }
                }
                
                if (val.HasMember(FREQUENCY_KEY.c_str())){
                    static_cast<veValueOscillate *>(value)->setFrequency(val[FREQUENCY_KEY.c_str()].GetDouble());
                }
                
                if (val.HasMember(PHASE_KEY.c_str())){
                    static_cast<veValueOscillate *>(value)->setPhase(val[PHASE_KEY.c_str()].GetDouble());
                }
                
                if (val.HasMember(BASE_KEY.c_str())){
                    static_cast<veValueOscillate *>(value)->setBase(val[BASE_KEY.c_str()].GetDouble());
                }
                
                if (val.HasMember(AMPLITUDE_KEY.c_str())){
                    static_cast<veValueOscillate *>(value)->setAmplitude(val[AMPLITUDE_KEY.c_str()].GetDouble());
                }
            }
        }
        return value;
    }

private:

	Document *_doucument;
    veParticleSystem *_particleSystem;
	veSceneManager *_sceneManager;
	std::string _name;
};

VE_READERWRITER_REG("veparticle", veFileReaderWriterPARTICLE);