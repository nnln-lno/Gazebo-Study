#ifndef GZ_SIM_SYSTEMS_MINHOAPPLYJOINTFORCE_HH_
#define GZ_SIM_SYSTEMS_MINHOAPPLYJOINTFORCE_HH_

#include <gz/sim/System.hh>
#include <memory>

namespace gz{
    
    namespace sim{

        inline namespace GZ_SIM_VERSION_NAMESPACE{

            namespace systems
            {
                class MyApplyJointForcePrivate;

                /// \brief This system applies a force to the first axis of a specified joint.
                ///
                /// ## Components
                ///
                /// This system uses the following components:
                ///
                /// - minho::sim::components::JointForceCmd: A std::vector of force commands
                ///   of type `double`. Only element `[0]` is used, for applying force to
                ///   the specified joint.
                
                class MyApplyJointForce :
                    public System,
                    public ISystemConfigure,
                    public ISystemPreUpdate
                {

                    public : MyApplyJointForce();

                    public : ~MyApplyJointForce() override = default; // 부모 소멸자가 가상 소멸자로 보장된다.

                    public : void Configure(const Entity &_entity,
                                            const std::shared_ptr<const sdf::Element> &_sdf,
                                            EntityComponentManager &_ecm,
                                            EventManager &_eventMgr) override;

                    public : void PreUpdate(
                        const gz::sim::UpdateInfo &_info,
                        gz::sim::EntityComponentManager &_ecm) override;                    

                    public : std::unique_ptr<MyApplyJointForcePrivate> dataPtr;

                };
            }
        }
    }
}

#endif