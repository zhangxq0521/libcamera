# SPDX-License-Identifier: CC0-1.0

subdir('tracepoints')

libcamera_tracepoint_header = custom_target(
    'tp_header',
    input: [ 'tracepoints.h.in', tracepoint_files ],
    output: 'tracepoints.h',
    command: [ gen_tracepoints_header, '@OUTPUT@', '@INPUT@' ],
)

libcamera_internal_headers = files([
    'byte_stream_buffer.h',
    'camera_controls.h',
    'camera_sensor.h',
    'control_serializer.h',
    'control_validator.h',
    'device_enumerator.h',
    'device_enumerator_sysfs.h',
    'device_enumerator_udev.h',
    'event_dispatcher.h',
    'event_dispatcher_poll.h',
    'event_notifier.h',
    'file.h',
    'formats.h',
    'ipa_context_wrapper.h',
    'ipa_manager.h',
    'ipa_module.h',
    'ipa_proxy.h',
    'ipc_unixsocket.h',
    'log.h',
    'media_device.h',
    'media_object.h',
    'message.h',
    'pipeline_handler.h',
    'process.h',
    'pub_key.h',
    'semaphore.h',
    'sysfs.h',
    'thread.h',
    'timer.h',
    'utils.h',
    'v4l2_controls.h',
    'v4l2_device.h',
    'v4l2_pixelformat.h',
    'v4l2_subdevice.h',
    'v4l2_videodevice.h',
])
