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

#ifndef RMF_TASK_ROS2__TASK_PROFILE_HPP
#define RMF_TASK_ROS2__TASK_PROFILE_HPP

#include <unordered_map>
#include <rmf_traffic/Time.hpp>
#include <rmf_traffic_ros2/Time.hpp>

#include <rmf_task_msgs/msg/task_profile.hpp>
#include <rmf_task_msgs/msg/dispatch_request.hpp>
#include <rmf_task_msgs/msg/dispatch_status.hpp>

namespace rmf_task_ros2 {

//==============================================================================
enum class TaskType { 
  Station,
  Loop,
  Delivery,
  Charging,
  Cleaning,
  Patrol
};

//==============================================================================
using TaskProfileMsg = rmf_task_msgs::msg::TaskProfile;
using TaskID = std::string;

//==============================================================================
struct TaskProfile
{
  using TaskParams = std::unordered_map<std::string, std::string>;
  
  TaskID task_id;
  rmf_traffic::Time submission_time;
  TaskType task_type;
  rmf_traffic::Time start_time;
  TaskParams params;

  bool operator==(const TaskProfile& tsk) const
  {
    return (this->task_id == tsk.task_id);
  }

  bool operator<(const TaskProfile& tsk) const
  {
    return (this->task_id < tsk.task_id);
  }
};

//==============================================================================
using RequestMsg = rmf_task_msgs::msg::DispatchRequest;
using StatusMsg = rmf_task_msgs::msg::DispatchStatus;
//==============================================================================

struct TaskStatus
{
  enum class State : uint8_t
  {
    Pending = StatusMsg::PENDING,
    Queued = StatusMsg::ACTIVE_QUEUED,
    Executing = StatusMsg::ACTIVE_EXECUTING,
    Completed = StatusMsg::TERMINAL_COMPLETED,
    Failed = StatusMsg::TERMINAL_FAILED,
    Canceled = StatusMsg::TERMINAL_CANCELED
  };

  std::string fleet_name;
  TaskProfile task_profile;
  rmf_traffic::Time start_time;
  rmf_traffic::Time end_time;
  std::string robot_name;
  std::string status; // verbose msg
  State state = State::Pending; // default
};

using TaskStatusPtr = std::shared_ptr<TaskStatus>;

//==============================================================================
TaskProfile convert(const TaskProfileMsg& from);

//==============================================================================
TaskProfileMsg convert(const TaskProfile& from);

// ==============================================================================
TaskStatus convert(const StatusMsg& from);

// ==============================================================================
StatusMsg convert(const TaskStatus& from);

} // namespace rmf_task_ros2

#endif // RMF_TASK_ROS2__TASK_PROFILE_HPP
