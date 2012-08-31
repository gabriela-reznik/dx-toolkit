#include <fstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include "dxjson/dxjson.h"
#include "dxcpp/dxcpp.h"

using namespace std;
using namespace dx;

// Return a JSON Array with a single random number between [0,99]: "[rand_number]"
JSON returnRandomRow() {
  JSON j(JSON_ARRAY);
  j.push_back(std::rand() % 100); // push a random number between 0-99 (inclusive)
  return j;
}

int main() { 
  // Read app input from job_input.json
  JSON input, output(JSON_HASH);
  ifstream ifs("job_input.json");
  input.read(ifs);
  
  // Get number of rows from input hash
  int numRows = input["numRows"].get<int>();
  
  ////////////////////
  // Create a GTable
  ////////////////////
  vector<JSON> columns;
  // Create the column spec for this gtable
  // We just have one int32 column ("rand_value")
  columns.push_back(DXGTable::columnDesc("rand_value", "int32"));
  DXGTable gtable = DXGTable::newDXGTable(columns);
  
  ////////////////////////
  // Add values to GTable
  ////////////////////////
  for (int i = 0; i < numRows; ++i)
    gtable.addRows(JSON::parse("[" + returnRandomRow().toString() + "]"));
  
  /////////////////////
  // Close the gtable
  ////////////////////
  gtable.close(true); // Block until gtable is closed

  /////////////////////////////////////////
  // Now read the gtable asynchorounsly
  /////////////////////////////////////////
 
  // We can optimize iteration over all rows of a
  // a gtable by using startLinearQuery() and getNextChunk()
  // construct (which fetches other chunks asynchronously in background)
  // while you process the previous chunk.
  // Call to startLinearQuery() signals that background fetching
  // of chunk should be started 
  // getNextChunk() can be used to get chunk in a sequential manner
  // See documentation for details about input params, etc
  gtable.startLinearQuery(JSON::parse("[\"rand_value\"]"), 0, numRows, (numRows/10 + 1));
  JSON chunk;
  int64_t sum = 0;
  while(gtable.getNextChunk(chunk)) {
    for (int i = 0; i < chunk.size(); ++i)
      sum += chunk[i][0].get<int>();
  }

  // Kill the background fetching threads
  gtable.stopLinearQuery();

  double avg = static_cast<double>(sum)/numRows;
  
  ///////////////////////
  // Create a new DXFile
  ///////////////////////
  // Create a file with name: "OutputFile"
  DXFile dxf = DXFile::newDXFile("", JSON::parse("{\"name\": \"OutputFile\"}"));
  // Add a tag "ResultsFile" to the file we just created
  dxf.addTags(JSON::parse("[\"ResultsFile\"]"));

  ////////////////////////////////////////////////////////////
  // Write some data to file (like result, etc), and close it
  ////////////////////////////////////////////////////////////
  dxf.write("This file is generated as a result of running this app");
  dxf.write("\nnumRows = " + boost::lexical_cast<string>(numRows));
  dxf.write("\nAverage = " + boost::lexical_cast<string>(avg));
  dxf.write("\nRandom gtable ID = " + gtable.getID());
  dxf.flush();
  dxf.close();
  
  // Populate the output hash
  output["rand_gtable"] = JSON::parse("{\"$dnanexus_link\": \"" + gtable.getID() + "\"}");
  output["col_avg"] = avg;
  output["results_file"] = JSON::parse("{\"$dnanexus_link\": \"" + dxf.getID() + "\"}");

  // Write app output to job_output.json
  ofstream ofs("job_output.json");
  ofs << (output.toString());
  ofs.close();

  return 0;
}
