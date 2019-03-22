/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2019, Google Inc.
 *
 * libcamera Camera API tests
 */

#include <iostream>

#include "camera_test.h"

using namespace std;

namespace {

class Capture : public CameraTest
{
protected:
	unsigned int completeBuffersCount_;
	unsigned int completeRequestsCount_;

	void bufferComplete(Request *request, Buffer *buffer)
	{
		if (buffer->status() != Buffer::BufferSuccess)
			return;

		completeBuffersCount_++;
	}

	void requestComplete(Request *request, const std::map<Stream *, Buffer *> &buffers)
	{
		if (request->status() != Request::RequestComplete)
			return;

		completeRequestsCount_++;

		/* Reuse the buffers for a new request. */
		request = camera_->createRequest();
		request->setBuffers(buffers);
		camera_->queueRequest(request);
	}

	int run()
	{
		Stream *stream = *camera_->streams().begin();
		std::set<Stream *> streams = { stream };
		std::map<Stream *, StreamConfiguration> conf =
			camera_->streamConfiguration(streams);
		StreamConfiguration *sconf = &conf.begin()->second;

		if (!configurationValid(streams, conf)) {
			cout << "Failed to read default configuration" << endl;
			return TestFail;
		}

		if (camera_->acquire()) {
			cout << "Failed to acquire the camera" << endl;
			return TestFail;
		}

		if (camera_->configureStreams(conf)) {
			cout << "Failed to set default configuration" << endl;
			return TestFail;
		}

		if (camera_->allocateBuffers()) {
			cout << "Failed to allocate buffers" << endl;
			return TestFail;
		}

		BufferPool &pool = stream->bufferPool();
		std::vector<Request *> requests;
		for (Buffer &buffer : pool.buffers()) {
			Request *request = camera_->createRequest();
			if (!request) {
				cout << "Failed to create request" << endl;
				return TestFail;
			}

			std::map<Stream *, Buffer *> map = { { stream, &buffer } };
			if (request->setBuffers(map)) {
				cout << "Failed to associating buffer with request" << endl;
				return TestFail;
			}

			requests.push_back(request);
		}

		completeRequestsCount_ = 0;
		completeBuffersCount_ = 0;

		camera_->bufferCompleted.connect(this, &Capture::bufferComplete);
		camera_->requestCompleted.connect(this, &Capture::requestComplete);

		if (camera_->start()) {
			cout << "Failed to start camera" << endl;
			return TestFail;
		}

		for (Request *request : requests) {
			if (camera_->queueRequest(request)) {
				cout << "Failed to queue request" << endl;
				return TestFail;
			}
		}

		EventDispatcher *dispatcher = CameraManager::instance()->eventDispatcher();

		Timer timer;
		timer.start(100);
		while (timer.isRunning())
			dispatcher->processEvents();

		if (completeRequestsCount_ <= sconf->bufferCount * 2) {
			cout << "Failed to capture enough frames (got "
			     << completeRequestsCount_ << " expected at least "
			     << sconf->bufferCount * 2 << ")" << endl;
			return TestFail;
		}

		if (completeRequestsCount_ != completeBuffersCount_) {
			cout << "Number of completed buffers and requests differ" << endl;
			return TestFail;
		}

		if (camera_->stop()) {
			cout << "Failed to stop camera" << endl;
			return TestFail;
		}

		if (camera_->freeBuffers()) {
			cout << "Failed to free buffers" << endl;
			return TestFail;
		}

		return TestPass;
	}
};

} /* namespace */

TEST_REGISTER(Capture);