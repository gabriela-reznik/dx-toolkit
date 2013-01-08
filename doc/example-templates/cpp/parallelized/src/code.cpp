/*
 * DX_APP_WIZARD_NAME DX_APP_WIZARD_VERSION
 * Generated by dx-app-wizard.
 *
 * Parallelized execution pattern: Your app will generate multiple
 * jobs to perform some computation in parallel, followed by a final
 * "postprocess" stage that will perform any additional computations
 * as necessary.
 *
 * See http://wiki.dnanexus.com/Developer-Portal for documentation and
 * tutorials on how to modify this file.
 *
 * By default, this template uses the DNAnexus C++ JSON library and
 * the C++ bindings.
 */

#include <iostream>
#include <vector>
#include <stdint.h>

#include "dxjson/dxjson.h"
#include "dxcpp/dxcpp.h"

using namespace std;
using namespace dx;

void postprocess() {
  JSON input;
  dxLoadInput(input);

  // You may want to copy and paste the logic to download and upload
  // files here as well if this stage receives file input and/or makes
  // file output.

  JSON output = JSON(JSON_HASH);
  dxWriteOutput(output);
}

void process() {
  JSON input;
  dxLoadInput(input);

  // You may want to copy and paste the logic to download and upload
  // files here as well if this stage receives file input and/or makes
  // file output.

  JSON output = JSON(JSON_HASH);
  dxWriteOutput(output);
}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    if (strcmp(argv[1], "process") == 0) {
      process();
      return 0;
    } else if (strcmp(argv[1], "postprocess") == 0) {
      postprocess();
      return 0;
    } else if (strcmp(argv[1], "main") != 0) {
      return 1;
    }
  }

  JSON input;
  dxLoadInput(input);

  // The variable *input* should now contain the input fields given to
  // the app(let), with keys equal to the input field names.
  //
  // For example, if an input field is of name "num" and class "int",
  // you can obtain the value via:
  //
  // int num = input["num"].get<int>();
  //
  // See http://wiki.dnanexus.com/dxjson for more details on how to
  // use the C++ JSON library.
DX_APP_WIZARD_INITIALIZE_INPUTDX_APP_WIZARD_DOWNLOAD_ANY_FILES
  // Split your work into parallel tasks.  As an example, the
  // following generates 10 subjobs running with the same dummy input.

  JSON process_input = JSON(JSON_HASH);
  process_input["input1"] = true;
  vector<DXJob> subjobs;
  for (int i = 0; i < 10; i++) {
    subjobs.push_back(DXJob::newDXJob(process_input, "process"));
  }

  // The following line creates the job that will perform the
  // "postprocess" step of your app.  If you give it any inputs that
  // use outputs from the "process" jobs, then it will automatically
  // wait for those jobs to finish before it starts running.  If you
  // do not need to give it any such inputs, you can explicitly state
  // the dependencies to wait for those jobs to finish by setting the
  // "depends_on" field to the list of subjobs to wait for (it accepts
  // either DXJob objects are string job IDs in the list).

  vector<JSON> process_jbors;
  for (int i = 0; i < subjobs.size(); i++) {
    process_jbors.push_back(subjobs[i].getOutputRef("output"));
  }
  JSON postprocess_input = JSON(JSON_HASH);
  postprocess_input["process_outputs"] = process_jbors;
  DXJob postprocess_job = DXJob::newDXJob(postprocess_input, "postprocess");
DX_APP_WIZARD_UPLOAD_ANY_FILES
  // If you would like to include any of the output fields from the
  // postprocess_job as the output of your app, you should return it
  // here using a reference.  If the output field in the postprocess
  // function is called "answer", you can set that in the output hash
  // as follows.
  //
  // output["app_output_field"] = postprocess_job.getOutputRef("answer");
  //
  // Tip: you can include in your output at this point any open
  // objects (such as gtables) which are closed by another entry
  // point that finishes later.  The system will check to make sure
  // that the output object is closed and will attempt to clone it
  // out as output into the parent container only after all subjobs
  // have finished.

  JSON output = JSON(JSON_HASH);
DX_APP_WIZARD_OUTPUT
  dxWriteOutput(output);

  return 0;
}
