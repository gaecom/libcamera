/* SPDX-License-Identifier: LGPL-2.1-or-later */
/*
 * Copyright (C) 2019-2021, Google Inc.
 *
 * camera_request.h - libcamera Android Camera Request Descriptor
 */
#ifndef __ANDROID_CAMERA_REQUEST_H__
#define __ANDROID_CAMERA_REQUEST_H__

#include <map>
#include <memory>
#include <mutex>
#include <vector>

#include <libcamera/base/class.h>

#include <libcamera/camera.h>
#include <libcamera/framebuffer.h>

#include <hardware/camera3.h>

#include "camera_metadata.h"
#include "camera_worker.h"

class CameraBuffer;
class CameraStream;

class Camera3RequestDescriptor
{
public:
	enum class Status {
		Success,
		Error,
	};

	struct StreamBuffer {
		CameraStream *stream;
		buffer_handle_t *camera3Buffer;
		std::unique_ptr<libcamera::FrameBuffer> frameBuffer;
		int fence;
		Status status;
		libcamera::FrameBuffer *internalBuffer;
		const libcamera::FrameBuffer *srcBuffer;
		std::unique_ptr<CameraBuffer> dstBuffer;
		Camera3RequestDescriptor *request;
	};

	/* Keeps track of streams requiring post-processing. */
	std::map<CameraStream *, StreamBuffer *> pendingStreamsToProcess_;
	std::mutex streamsProcessMutex_;

	Camera3RequestDescriptor(libcamera::Camera *camera,
				 const camera3_capture_request_t *camera3Request);
	~Camera3RequestDescriptor();

	bool isPending() const { return !complete_; }

	uint32_t frameNumber_ = 0;

	std::vector<StreamBuffer> buffers_;

	CameraMetadata settings_;
	std::unique_ptr<CaptureRequest> request_;
	std::unique_ptr<CameraMetadata> resultMetadata_;

	bool complete_ = false;
	Status status_ = Status::Success;

private:
	LIBCAMERA_DISABLE_COPY(Camera3RequestDescriptor)
};

#endif /* __ANDROID_CAMERA_REQUEST_H__ */
