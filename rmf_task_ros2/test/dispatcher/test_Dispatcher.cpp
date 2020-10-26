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

#include <rmf_utils/optional.hpp>
#include <rmf_task_ros2/dispatcher/Dispatcher.hpp>
#include <rclcpp/rclcpp.hpp>

#include <chrono>
#include <thread>
#include <rmf_utils/catch.hpp>

namespace rmf_task_ros2 {
namespace dispatcher {

// //==============================================================================
auto task_time = std::chrono::steady_clock::now();
TaskProfile task_profile1{"placenholder_id", task_time, TaskType::Station };
TaskProfile task_profile2{"placenholder_id", task_time, TaskType::Cleaning };

//==============================================================================
SCENARIO("Dispatcehr API Test", "[Dispatcher]")
{
  rclcpp::shutdown(); // todo: temp hack
  auto dispatcher = Dispatcher::make("test_dispatcher_node");

  auto spin_thread = std::thread(
    [&dispatcher]()
    {
      dispatcher->spin();
    });
  spin_thread.detach();

  WHEN("Add 1 and cancel task")
  {
    std::cout << "Here1" << std::endl;

    // add task
    auto id = dispatcher->submit_task(task_profile1);
    REQUIRE(dispatcher->active_tasks()->size() == 1);
    REQUIRE(dispatcher->terminated_tasks()->size() == 0);
    REQUIRE(dispatcher->get_task_state(id) == TaskStatus::State::Pending);

    // check random id
    REQUIRE(!(dispatcher->get_task_state("non_existence_id")));

    // cancel task
    dispatcher->cancel_task(id);
    REQUIRE(dispatcher->active_tasks()->size() == 0);
    REQUIRE(dispatcher->terminated_tasks()->size() == 1);
    REQUIRE(dispatcher->get_task_state(id) == TaskStatus::State::Canceled);
  }

//==============================================================================

  // test on change fn callback
  int change_times = 0;
  TaskProfile test_taskprofile;
  dispatcher->on_change(
    [&change_times, &test_taskprofile](const TaskStatusPtr status)
    {
      test_taskprofile = status->task_profile;
      std::cout << " On change! > " << test_taskprofile.task_id
                << "  " << (int)status->state << std::endl;
      change_times++;
    }
  );

  WHEN("Track Task till Bidding Failed")
  {
    // Submit first task and wait for bidding
    auto id = dispatcher->submit_task(task_profile1);
    task_profile1.task_id = id; // update id
    REQUIRE(dispatcher->active_tasks()->size() == 1);
    REQUIRE(dispatcher->get_task_state(id) == TaskStatus::State::Pending);

    // Default 2s timeout, wait 3s for timetout, should fail here
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    REQUIRE(dispatcher->get_task_state(id) == TaskStatus::State::Failed);
    REQUIRE(dispatcher->terminated_tasks()->size() == 1);
    REQUIRE(test_taskprofile == task_profile1);
    REQUIRE(change_times == 2); // add and failed

    // Submit another task
    id = dispatcher->submit_task(task_profile2);
    task_profile2.task_id = id;
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    REQUIRE(dispatcher->terminated_tasks()->size() == 2);
    REQUIRE(test_taskprofile == task_profile2);
    REQUIRE(change_times == 4); // add and failed x2
  }

  // WHEN("Full Dispatch cycle")
  // {
  //   // todo
  // }
}

} // namespace action
} // namespace rmf_task_ros2