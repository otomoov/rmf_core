/*
 * Copyright (C) 2020 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#ifndef RMF_TASK_ROS2__DISPATCHER__NODE_HPP
#define RMF_TASK_ROS2__DISPATCHER__NODE_HPP

#include <rclcpp/node.hpp>
#include <rclcpp/rclcpp.hpp>
#include <rmf_utils/impl_ptr.hpp>
#include <rmf_utils/optional.hpp>

#include <rmf_task_ros2/StandardNames.hpp>
#include <rmf_task_ros2/bidding/Auctioneer.hpp>
#include <rmf_task_ros2/action/ActionClient.hpp>

namespace rmf_task_ros2 {
namespace dispatcher {

//==============================================================================
using DispatchTasks = std::map<TaskID, TaskStatusPtr>;
using DispatchTasksPtr = std::shared_ptr<DispatchTasks>;
using DispatchState = TaskStatus::State;

//==============================================================================
class Dispatcher : public std::enable_shared_from_this<Dispatcher>
{
public:

  /// Initialize an rclcpp context and make an dispatcher instance. This will
  /// instantiate an rclcpp::Node, a task dispatcher node. Dispatcher node will
  /// allow you to dispatch submitted task to the best fleet/robot within RMF.
  ///
  /// \param[in] node
  ///   The ROS 2 node to manage the Dispatching of Task
  ///
  /// \sa make()
  static std::shared_ptr<Dispatcher> init_and_make(
    const std::string dispatcher_node_name);

  /// Similarly this will init the dispatcher, but you will also need to init
  /// rclcpp via rclcpp::init(~).
  ///
  /// \param[in] node
  ///   The ROS 2 node to manage the Dispatching of Task
  ///
  /// \sa make()
  static std::shared_ptr<Dispatcher> make(
    const std::string dispatcher_node_name);

  /// Submit task to dispatcher node. Calling this function will immediately
  /// trigger the bidding process, then the task "action". Once submmitted,
  /// Task State will be in 'Pending' State, till the task is awarded to a fleet
  /// then the state will turn to 'Queued'
  ///
  /// \param [in] task
  ///   Submit a task to dispatch
  ///
  /// \return task_id
  ///   self-generated task_id
  TaskID submit_task(
    const TaskProfile& task);

  /// Cancel task which was previously submitted to this dispatcher. This
  /// will Terminate the task with a State of: Canceled. If a task is being
  /// Queued or executing, cancel_task will only mean sending a cancel req to
  /// the respective FA. The fleet adapter will need to send a cancel status
  ///
  /// \param [in] task_id
  ///   Task to cancel
  ///
  /// \return true if success
  bool cancel_task(const TaskID& task_id);

  /// Check the state of a submited task. It can be either active or terminated
  ///
  /// \param [in] task_id
  ///   request task id
  ///
  /// \return State of the task
  rmf_utils::optional<DispatchState> get_task_state(
    const TaskID& task_id);

  /// Get active tasks map list handled by dispatcher
  ///
  /// \return ptr to a map of active tasks
  const DispatchTasksPtr active_tasks() const;

  /// Get terminated tasks map list
  ///
  /// \return ptr to a map of terminated tasks
  const DispatchTasksPtr terminated_tasks() const;

  using StatusCallback = std::function<void(const TaskStatusPtr status)>;

  /// Trigger this callback when a task status is changed. This will return the
  /// Changed task status.
  ///
  /// \param [in] callback function
  void on_change(StatusCallback on_change_fn);

  /// Change the evaluator method used in auctioneer during the bidding process
  ///
  /// \param [in] evaluator
  ///   evaluator used to select the best bid from fleets
  void evaluator(std::shared_ptr<bidding::Auctioneer::Evaluator> evaluator);

  /// Get the rclcpp::Node that this dispatcher will be using for communication.
  std::shared_ptr<rclcpp::Node> node();

  /// spin dispatcher node
  void spin();

  ~Dispatcher();

  class Implementation;

private:
  Dispatcher();
  rmf_utils::impl_ptr<Implementation> _pimpl;
};

} // namespace dispatcher


} // namespace rmf_task_ros2

#endif // RMF_TASK_ROS2__DISPATCHER__NODE_HPP