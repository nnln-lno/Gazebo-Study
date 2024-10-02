#include "MinhoApplyJointForce.hpp"

#include <gz/msgs/double.pb.h>

#include <string>

#include <gz/common/Profiler.hh>
#include <gz/plugin/Register.hh>
#include <gz/transport/Node.hh>

#include "gz/sim/components/JointForceCmd.hh"
#include "gz/sim/Model.hh"
#include "gz/sim/Util.hh"

using namespace gz;
using namespace sim;
using namespace systems;

class gz::sim::systems::MyApplyJointForcePrivate
{
    /// \brief Callback for joint force subscription
    /// \param[in] _msg Joint force message
    public : void OnCmdForce(const msgs::Double &_msg);

    /// \brief Gazebo communication node.
    public : transport::Node node;

    /// \brief Joint Entity -> 엔티티(조인트)에 접근하기 위한 멤버 변수 선언
    public : Entity jointEntity;

    /// \brief Joint name
    public : std::string jointName;

    /// \brief Commanded joint force
    public : double jointForceCmd;

    /// \brief mutext to protect jointForceCmd
    public : std::mutex jointForceCmdMutex;

    /// \brief Model interface
    /// 엔티티는 월드가 로드될 때 생성될 수 있으며, 존재하지 않는 경우 런타임에 엔티티를 생성할 수도 있다.
    /// Joint의 경우 대부분 월드를 정의하는 SDF 파일에 정의되어 있으며, 런타임 시에는 이름을 통해 해당 Joint를 찾기만 하면 된다.
    public : Model model{kNullEntity};
};

MyApplyJointForce::MyApplyJointForce() : dataPtr(std::make_unique<MyApplyJointForcePrivate>())
{    
    
}

void MyApplyJointForce::Configure(const Entity &_entity,
    const std::shared_ptr<const sdf::Element> &_sdf,
    EntityComponentManager &_ecm,
    EventManager &/*_eventMgr*/)
{

    /// ApplyJointForce는 SDF 파일의 <model> 태그에 지정되어 있으므로, Configure()가 호출될 때 모델 엔티티에 접근할 수 있으며
    /// 또한 이를 통해 gz::sim::Model 추출할 수 있다.

    this->dataPtr->model = Model(_entity);

    if (!this->dataPtr->model.Valid(_ecm))
    {
        gzerr << "MyApplyJointForce plugin should be attached to a model entity. Failed to initialize." << std::endl;
        return;
    }

    auto sdfClone = _sdf->Clone();

    // Get Param from SDF
    auto sdfElem = sdfClone->GetElement("joint_name");
    if (sdfElem)
    {
        this->dataPtr->jointName = sdfElem->Get<std::string>();
    }

    if (this->dataPtr->jointName == "")
    {
        gzerr << "MyApplyJointForce found an empty jointName parameter. Failed to initialize." << std::endl;
        return;
    }

    // Subscribe to commands
    auto topic = transport::TopicUtils::AsValidTopic("/model/" + this->dataPtr->model.Name(_ecm) 
                    + "/joint/" + this->dataPtr->jointName + "/cmd_force");

    if(topic.empty())
    {
        gzerr << "Failed to create valid topic for [" << this->dataPtr->jointName << "]" << std::endl;
        return;
    }

    this->dataPtr->node.Subscribe(topic, &MyApplyJointForcePrivate::OnCmdForce, this->dataPtr.get());

    gzmsg << "MyApplyJointForce subscribing to Double messages on [" << topic << "]" << std::endl;
}

void MyApplyJointForce::PreUpdate(const UpdateInfo &_info,
    EntityComponentManager &_ecm)
{
    GZ_PROFILE("MyApplyJointForce::PreUpdate");

    if(_info.dt < std::chrono::steady_clock::duration::zero())
    {
        gzwarn << "Detected jump back in time [" << std::chrono::duration<double>(_info.dt).count()
               << "s]. System may not work properly." << std::endl;
    }

    /// Model 객체를 사용하여, PreUpdate()가 호출될 때 조인트를 이름으로 찾을 수 있다. 이를통해 Joint 엔티티를 얻을 수 있음.
    // If the joint hasn't been identified yet, look for it
    if(this->dataPtr->jointEntity == kNullEntity)
    {
        this->dataPtr->jointEntity =
            this->dataPtr->model.JointByName(_ecm, this->dataPtr->jointName);
    }

    // Even after look for it, there's no joint, return.
    if(this->dataPtr->jointEntity == kNullEntity)
    {
        return;
    }

    // Nothing left to do if paused.
    if(_info.paused)
    {
        return;
    }

    // Update joint force
    auto force = _ecm.Component<components::JointForceCmd>(
        this->dataPtr->jointEntity
    );

    // 공유자원의 동시 접근을 막기 위한 Lock, lock_guard는 객체 생성시 lock 되며 소멸시에 unlock 된다.
    std::lock_guard<std::mutex> lock(this->dataPtr->jointForceCmdMutex);

    // 엔티티 Handle을 얻고 나면, 관련된 컴포넌트를 수정할 수 있다. 이 컴포넌트는 월드가 로드될 때 생성되었을 수도 있고 런타임에 생성되었을 수도 있다.
    // 이 경우, 위에서 찾은 jointEntity를 사용하여 JointForceCmd 컴포넌트를 찾아 수정한다. 이를 통해 조인트에 가하는 힘 명령을 적용 가능하다.
    // PreUpdate() 에서 컴포넌트를 찾는 방법은 아래와 같다.

    // 컴포넌트가 아직 존재하지 않는 경우, 해당 컴포넌트를 생성하고 수정할 수 있다. 
    if(force == nullptr)
    {
        _ecm.CreateComponent(
            this->dataPtr->jointEntity,
            components::JointForceCmd({this->dataPtr->jointForceCmd})
        );
    }
    else
    {
        force->Data()[0] += this->dataPtr->jointForceCmd;
    }

}

void MyApplyJointForcePrivate::OnCmdForce(const msgs::Double &_msg)
{
    std::lock_guard<std::mutex> lock(this->jointForceCmdMutex);
    this->jointForceCmd = _msg.data();
}

GZ_ADD_PLUGIN(MyApplyJointForce,
                System,
                MyApplyJointForce::ISystemConfigure,
                MyApplyJointForce::ISystemPreUpdate)

GZ_ADD_PLUGIN_ALIAS(MyApplyJointForce,
                        "gz::sim::systems::MyApplyJointFoce")
