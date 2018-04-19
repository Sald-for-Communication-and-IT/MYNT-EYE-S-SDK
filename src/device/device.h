#ifndef MYNTEYE_DEVICE_H_  // NOLINT
#define MYNTEYE_DEVICE_H_
#pragma once

#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "mynteye/mynteye.h"
#include "mynteye/types.h"

#include "internal/callbacks.h"

MYNTEYE_BEGIN_NAMESPACE

namespace uvc {

struct device;

}  // namespace uvc

struct DeviceInfo;

class Channels;
class Motions;
class Streams;

class Device {
 public:
  using stream_callback_t = device::StreamCallback;
  using motion_callback_t = device::MotionCallback;

  using stream_callbacks_t = std::map<Stream, stream_callback_t>;

  Device(const Model &model, std::shared_ptr<uvc::device> device);
  virtual ~Device();

  static std::shared_ptr<Device> Create(
      const std::string &name, std::shared_ptr<uvc::device> device);

  Model GetModel() const {
    return model_;
  }

  bool Supports(const Stream &stream) const;
  bool Supports(const Capabilities &capability) const;
  bool Supports(const Option &option) const;

  const std::vector<StreamRequest> &GetStreamRequests(
      const Capabilities &capability) const;
  void ConfigStreamRequest(
      const Capabilities &capability, const StreamRequest &request);

  std::shared_ptr<DeviceInfo> GetInfo() const;
  std::string GetInfo(const Info &info) const;

  ImgIntrinsics GetImgIntrinsics() const;
  ImgExtrinsics GetImgExtrinsics() const;
  void SetImgIntrinsics(const ImgIntrinsics &in);
  void SetImgExtrinsics(const ImgExtrinsics &ex);

  ImuIntrinsics GetImuIntrinsics() const;
  ImuExtrinsics GetImuExtrinsics() const;
  void SetImuIntrinsics(const ImuIntrinsics &in);
  void SetImuExtrinsics(const ImuExtrinsics &ex);

  void LogOptionInfos() const;
  OptionInfo GetOptionInfo(const Option &option) const;

  std::int32_t GetOptionValue(const Option &option) const;
  void SetOptionValue(const Option &option, std::int32_t value);

  bool RunOptionAction(const Option &option) const;

  void SetStreamCallback(const Stream &stream, stream_callback_t callback);
  void SetMotionCallback(motion_callback_t callback);

  bool HasStreamCallback(const Stream &stream) const;
  bool HasMotionCallback() const;

  virtual void Start(const Source &source);
  virtual void Stop(const Source &source);

  void WaitForStreams();

  std::vector<device::StreamData> GetStreamDatas(const Stream &stream);
  device::StreamData GetLatestStreamData(const Stream &stream);

  void EnableMotionDatas(
      std::size_t max_size = std::numeric_limits<std::size_t>::max());
  std::vector<device::MotionData> GetMotionDatas();

 protected:
  std::shared_ptr<uvc::device> device() const {
    return device_;
  }

  std::shared_ptr<DeviceInfo> device_info() const {
    return device_info_;
  }

  const StreamRequest &GetStreamRequest(const Capabilities &capability);

  virtual void StartVideoStreaming();
  virtual void StopVideoStreaming();

  virtual void StartMotionTracking();
  virtual void StopMotionTracking();

  virtual std::vector<Stream> GetKeyStreams() const = 0;

  bool video_streaming_;
  bool motion_tracking_;

 private:
  Model model_;
  std::shared_ptr<uvc::device> device_;
  std::shared_ptr<DeviceInfo> device_info_;

  ImgIntrinsics img_intrinsics_;
  ImgExtrinsics img_extrinsics_;
  ImuIntrinsics imu_intrinsics_;
  ImuExtrinsics imu_extrinsics_;

  stream_callbacks_t stream_callbacks_;
  motion_callback_t motion_callback_;

  std::shared_ptr<Streams> streams_;

  std::map<Capabilities, StreamRequest> stream_config_requests_;

  std::mutex mtx_streams_;

  std::shared_ptr<Channels> channels_;

  std::shared_ptr<Motions> motions_;

  void ReadAllInfos();

  void WriteDeviceInfo(const DeviceInfo &device_info);
  void WriteImgParams(
      const ImgIntrinsics &intrinsics, const ImgExtrinsics &extrinsics);
  void WriteImuParams(
      const ImuIntrinsics &intrinsics, const ImuExtrinsics &extrinsics);

  // friend DeviceWriter;
};

MYNTEYE_END_NAMESPACE

#endif  // MYNTEYE_DEVICE_H_ NOLINT
