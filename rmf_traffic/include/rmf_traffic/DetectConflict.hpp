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

#ifndef RMF_TRAFFIC__DETECTCONFLICT_HPP
#define RMF_TRAFFIC__DETECTCONFLICT_HPP

#include <rmf_traffic/Trajectory.hpp>
#include <rmf_traffic/Profile.hpp>
#include <exception>

#include <fcl/continuous_collision.h>
#include <fcl/ccd/motion.h>
#include <fcl/collision.h>

namespace rmf_traffic {

//==============================================================================
class invalid_trajectory_error : public std::exception
{
public:

  const char* what() const noexcept override;

  class Implementation;
private:
  invalid_trajectory_error();
  rmf_utils::impl_ptr<Implementation> _pimpl;
};

//==============================================================================
class DetectConflict
{
public:

  enum class Interpolate : uint16_t
  {
    CubicSpline
  };

  /// Checks if there are any conflicts between the two trajectories.
  ///
  /// \return true if a conflict exists between the trajectories, false
  /// otherwise.
  static rmf_utils::optional<rmf_traffic::Time> between(
    const Profile& profile_a,
    const Trajectory& trajectory_a,
    const Profile& profile_b,
    const Trajectory& trajectory_b,
    Interpolate interpolation = Interpolate::CubicSpline);

  class Implementation;

  static rmf_utils::optional<fcl::FCL_REAL> check_collision_fcl(
    const geometry::FinalConvexShape& shape_a,
    const std::shared_ptr<fcl::SplineMotion>& motion_a,
    const geometry::FinalConvexShape& shape_b,
    const std::shared_ptr<fcl::SplineMotion>& motion_b);

  static rmf_utils::optional<double> check_collision_piecewise_sweep(
    double radius_a,
    const std::array<Eigen::Vector4d, 3>& motion_a,
    double radius_b,
    const std::array<Eigen::Vector4d, 3>& motion_b);
};




} // namespace rmf_traffic

#endif // RMF_TRAFFIC__DETECTCONFLICT_HPP
