
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/header.hpp"
#include <string>

namespace message_trace {
class MessageTrace : public rclcpp::Node {
public:
  MessageTrace(std::string node_name, std::string trace_point_name);
  void update(std_msgs::msg::Header *msg, std_msgs::msg::Header *msg_ = nullptr);

private:
  int update_type_;
  OnSetParametersCallbackHandle::SharedPtr param_;
};
} // namespace message_trace
