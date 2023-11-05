#pragma once

#include <memory>
#include <iostream>
#include "SmartPortHandler.h"

class FakeSmartPortHandler : public SmartPortHandler {

public:
  FakeSmartPortHandler() {
    std::cout << "Creating FakeSmartPortHandler" << std::endl;
  }

  virtual std::unique_ptr<Response> status(StatusRequest* request) override;
  // virtual Response* read_block(ReadBlockRequest* request) override;
  // virtual Response* write_block(WriteBlockRequest* request) override;
  // virtual Response* format(FormatRequest* request) override;
  // virtual Response* control(ControlRequest* request) override;
  // virtual Response* init(InitRequest* request) override;
  // virtual Response* open(OpenRequest* request) override;
  // virtual Response* close(CloseRequest* request) override;
  // virtual Response* read(ReadRequest* request) override;
  // virtual Response* write(WriteRequest* request) override;
  // virtual Response* reset(ResetRequest* request) override;

};