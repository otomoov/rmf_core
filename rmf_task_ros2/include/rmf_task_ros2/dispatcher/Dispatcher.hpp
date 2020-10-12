
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
#include <rmf_task_ros2/StandardNames.hpp>
#include <rmf_task_ros2/bidding/Auctioneer.hpp>

#include <rmf_task_ros2/action/ActionInterface.hpp>

#include <rmf_utils/optional.hpp>

namespace rmf_task_ros2 {
namespace dispatcher {

//==============================================================================
// (TODO) new dispatcher lib

enum class DispatchState{
  // inherited from Action Task State
  Invalid,    // null
  Queued,     // active
  Executing,  // active
  Completed,  // active
  Failed,     // terminated
  Canceled,   // terminated

  // Additional State
  Bidding = 10  // active
};

struct DispatchTask
{
  TaskProfile task_profile;
  DispatchState dispatch_state;
  rmf_utils::optional<bidding::Submission> winner;
  // rmf_utils::optional<action::TaskMsg> task_status; TODO
  double progress; // 0 - 1.0
};
using DispatchTasks = std::map<TaskID, DispatchTask>;
using DispatchTasksPtr = std::shared_ptr<DispatchTasks>;

//==============================================================================

class Dispatcher
{
public:
  /// Create the node of a Task Dispatcher, inherited of rclcpp node
  ///
  /// \return Pointer to the dispatcher node
  static std::shared_ptr<Dispatcher> make(
    std::shared_ptr<rclcpp::Node> node);
  
  /// submit task to dispatcher
  ///
  /// \param [in] task to dispatch
  /// \return generated task_id
  TaskID submit_task(const TaskProfile& task);

  /// cancel task in dispatcher
  ///
  /// \param [in] task to dispatch
  bool cancel_task(const TaskID& task_id);

  /// check status of a submited task
  ///
  /// \param [in] to identify task_id
  /// \return State of the task
  rmf_utils::optional<DispatchState> get_task_dispatch_state(
      const TaskID& task_id);

  /// Get current active tasks map list
  ///
  /// \return Ptr to active tasks
  DispatchTasksPtr get_active_tasks();

  /// Get current terminated tasks map list
  ///
  /// \return Ptr to terminated tasks
  DispatchTasksPtr get_terminated_tasks();

private:
  std::shared_ptr<rclcpp::Node> _node;
  std::shared_ptr<bidding::Auctioneer> _auctioneer;
  std::shared_ptr<action::TaskActionClient> _action_client;
  
  // todo, should use action task?
  using ActiveTaskState = std::pair<TaskProfile, DispatchState>;
  using ActiveTasksMap = std::map<TaskID, ActiveTaskState>;
  ActiveTasksMap _active_tasks;

  DispatchTasks _active_dispatch_tasks;
  DispatchTasks _terminal_dispatch_tasks; // todo limit size

  // Private constructor
  Dispatcher(std::shared_ptr<rclcpp::Node> node_);

  // Callback when a bidding winner is provided
  void receive_bidding_winner_cb(
      const TaskID& task_id, 
      const rmf_utils::optional<bidding::Submission> winner);

  // task action status callback
  void action_status_cb(
      const std::string& server_id, 
      const std::vector<action::TaskMsg>& tasks);

  // task action finish callback
  void action_finish_cb(
      const std::string& server_id,
      const action::TaskMsg& task, 
      const bool success);
};

} // namespace dispatcher
} // namespace rmf_task_ros2

#endif // RMF_TASK_ROS2__DISPATCHER__NODE_HPP
