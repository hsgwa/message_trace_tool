#include "message_trace/message_trace.hpp"
#include "std_msgs/msg/header.hpp"
#include <climits>
#include <string>

namespace message_trace {
std::string normalize(std::string name) {
  std::replace(name.begin(), name.end(), '/', '_');
  return name;
}
enum UpdateType { NONE, START, END, INIT };

MessageTrace::MessageTrace(std::string node_name, std::string trace_point_name)
    : Node(normalize(node_name) + normalize(trace_point_name), "message_trace") ,
      update_type_(NONE)
{
  std::string param_name = "update_type";

  auto parameter_callback =
      [&, param_name](const std::vector<rclcpp::Parameter> &parameters) {
        rcl_interfaces::msg::SetParametersResult result;
        result.successful = true;
        for (const auto &parameter : parameters) {
          if (parameter.get_name() == param_name) {
            auto value = parameter.as_string();
            RCLCPP_INFO(get_logger(), "%s", value.c_str());
            if (value == "none") {
              update_type_ = NONE;
            } else if (value == "start") {
              update_type_ = START;
            } else if (value == "end") {
              update_type_ = END;
            } else if (value == "init") {
              update_type_ = INIT;
            }
          } else {
            result.successful = false;
          }
        }
        return result;
      };
  declare_parameter<std::string>(param_name, "none");
  param_ = add_on_set_parameters_callback(parameter_callback);
}

void MessageTrace::update(std_msgs::msg::Header *msg,
                          std_msgs::msg::Header *msg_) {
  if (msg_) {
    // msg.stamp = min(msg.stamp, msg_.stamp)
    if (msg->stamp.sec > msg_->stamp.sec ||
        (msg->stamp.sec == msg_->stamp.sec && msg->stamp.nanosec > msg_->stamp.nanosec))  {
      msg->stamp.sec = msg_->stamp.sec;
      msg->stamp.nanosec = msg_->stamp.nanosec;
    }
  }
  if (update_type_ == START) {
    msg->stamp = now();
  } else if (update_type_ == END) {
    auto stamp = msg->stamp;
    msg->stamp = now();
    msg->stamp.sec = msg->stamp.sec - stamp.sec;
    if (msg->stamp.nanosec > stamp.nanosec) {
      msg->stamp.nanosec = msg->stamp.nanosec - stamp.nanosec;
    } else {
      msg->stamp.nanosec = 1e9 + msg->stamp.nanosec - stamp.nanosec;
      msg->stamp.sec = msg->stamp.sec - 1;
    }
  } else if (update_type_ == INIT) {
    msg->stamp.sec = INT32_MAX;
    msg->stamp.nanosec = UINT32_MAX;
  }
}

} // namespace message_trace
