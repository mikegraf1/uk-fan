
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <memory>
#include <chrono>
#include <json/reader.h>
#include <json/value.h>
#include <curl/curl.h>

static int usage(int /*argc*/, char* argv[]) {
  using std::cout;
  cout << "usage: " << argv[0] << '\n';
  cout << "usage: " << argv[0] << " -f <file to get list of rover images date from>\n";
  cout << "  or\n";
  cout << "       " << argv[0] << " -d <date of image as [dd-MON-yy]>\n\n";
  cout << "       Examples:\n";
  cout << "       " << argv[0] << " -f MarsDates.txt    (get all images in file)\n";
  cout << "       " << argv[0] << " -d 15-Dec-17 (get image by date)\n";
  cout << "       " << argv[0] << " -d  1-Mar-16 (get image by date)\n";
  cout << "       " << argv[0] << " -d 22-Feb-15 (get image by date)\n";
  cout << "       " << argv[0] << " -d  3-Jul-17 (get image by date)\n";
  cout << "       " << argv[0] << " -d  5-Jul-17 (get image by date)\n";
  exit(0);
}

static bool check_usage(int argc, char* argv[], std::string& file, std::string& image_date) {
  int only_one = 0;
  int c;
  while ((c = getopt(argc, argv, "f:d:")) != -1) {
    switch (c) {
    case 'f':
      ++only_one;
      file = optarg;
      break;
    case 'd':
      ++only_one;
      image_date = optarg;
      break;
    case '?':
    default:
      return false;
    } 
  }
  if (only_one == 1) return true;
  else return false;
}

static std::string convert_date(const std::string image_date) {
  std::string day, month, year;
  size_t spos = 0;
  size_t epos = image_date.find('-');
  if (epos != std::string::npos) day = image_date.substr(0, epos - spos);
  spos = epos;
  spos++;
  epos = image_date.find('-', spos);
  if (epos != std::string::npos) month = image_date.substr(spos, epos - spos);
  epos++;
  year = image_date.substr(epos);
  if (month == "Jan") month="1";
  if (month == "Feb") month="2";
  if (month == "Mar") month="3";
  if (month == "Apr") month="4";
  if (month == "May") month="5";
  if (month == "Jun") month="6";
  if (month == "Jul") month="7";
  if (month == "Aug") month="8";
  if (month == "Sep") month="9";
  if (month == "Oct") month="10";
  if (month == "Nov") month="11";
  if (month == "Dec") month="12";
  int yr = stoi(year);
  yr = 2000 + yr;
  year = std::to_string(yr);
  std::string nimage_date = year + "-" + month + "-" + day;
  return nimage_date;
}

std::vector<std::string> output(const Json::Value & root) {
  std::vector<std::string> image_url_vec;
  const Json::Value photos = root["photos"];
  for (int index = 0; index < photos.size(); ++index ) {  // Iterates over the sequence elements. 
    std::cout << photos[index].get("img_src", "img_src").asString() << '\n';
    image_url_vec.push_back(photos[index].get("img_src", "img_src").asString()); 
  }
  return image_url_vec;
}

static size_t write_to_string(void *ptr, size_t size, size_t count, void *stream) {
  ((std::string*) stream)->append((char*) ptr, 0, size * count);
  return size * count;
}

static std::vector<std::string> curl_it(const std::string& image_date) {
  // input is dd-MON-yy
  // url requires yyyy-mm-dd
  // use date.h in the future TODO
  std::vector<std::string> image_vec;
//  CURLcode res;    
  //std::string url = "https://api.nasa.gov/mars-photos/api/v1/rovers/curiosity/photos?earth_date=2015-6-3&api_key=DEMO_KEY";
  std::string nimage_date = convert_date(image_date);
std::cout << "converted date: " << nimage_date << '\n';
//  nimage_date = "2015-6-3";
  std::string url = "https://api.nasa.gov/mars-photos/api/v1/rovers/curiosity/photos?earth_date=" + nimage_date + "&api_key=DEMO_KEY";
  CURL* curl = curl_easy_init();
std::cout << url << '\n';
#if 0
std::cout << url1 << '\n';
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Expect:");
  headers = curl_slist_append(headers, "Content-Type: application/json");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//  no pay load
//  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
//  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L);
#endif
    // Set remote URL.
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
std::cout << url.c_str() << '\n';
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
  // Don't bother trying IPv6, which would increase DNS resolution time.
  curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
  // Don't wait forever, time out after 10 seconds.
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
  // Follow HTTP redirects if necessary.
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  // Response information.
  int http_code{0};
  std::string http_data;
  // Hook up data handling function.
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
  // Hook up data container (will be passed as the last parameter to the
  // callback handling function).  Can be any pointer type, since it will
  // internally be passed as a void pointer.
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &http_data);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  //curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64; rv:58.0) Gecko/20100101 Firefox/58.0");
  // Run our HTTP GET command, capture the HTTP response code, and clean up.
  curl_easy_perform(curl);
//std::cout << "res: " << res << '\n';
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    std::cout <<"this is the response " << http_data << "\n";
//  std::cout <<"this is the response: " << http_data << "\n";
//  std::cout <<"this is the response size: " << http_data.size() << "\n";
//  std::cout <<"this is the http_code: " << http_code << "\n";
//  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  if (http_code == 200) {
//std::cout << "http_code: " << http_code << '\n';
    std::cout << "\nSuccessful response from " << url << std::endl;
    // Response looks good - done using Curl now.  Try to parse the results
    // and print them out.
    Json::Value json_data;
    Json::Reader json_reader;
    if (json_reader.parse(http_data, json_data)) {
//      std::cout << "Successfully parsed JSON data\n";
//      std::cout << "\nJSON data received:\n";
//      std::cout << json_data.toStyledString() << '\n';
      image_vec = output(json_data); 
    }
  }
  else {
    std::cout << "Images not found: " << http_code << '\n';
  }
  return image_vec;
}

static std::vector<std::string> retrieve_image_info(const std::string& image_date) {
  return curl_it(image_date);
}

std::string get_filename_from_url(const std::string& url) {
  int pos;
  pos = url.rfind('/'); pos++;
  return url.substr(pos);
}

static int retrieve_image(const std::string& image_path) {
//std::cout << image_path << '\n';
  // input is dd-MON-yy
  // url requires yyyy-mm-dd
  // use date.h in the future TODO
  //CURLcode res;    
  //std::string url = "https://api.nasa.gov/mars-photos/api/v1/rovers/curiosity/photos?earth_date=" + nimage_date + "&api_key=DEMO_KEY";
  std::string url = image_path;
  CURL* curl = curl_easy_init();
std::cout << url << '\n';
    // Set remote URL.
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  std::string filename = get_filename_from_url(url);
  FILE *storehere;
  storehere=fopen(filename.c_str(), "wb");
std::cout << url.c_str() << '\n';
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
  // Don't bother trying IPv6, which would increase DNS resolution time.
  curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
  // Don't wait forever, time out after 10 seconds.
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
  // Follow HTTP redirects if necessary.
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  // Response information.
  int http_code{0};
  //std::string http_data;
  // Hook up data handling function.
  //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
  // Hook up data container (will be passed as the last parameter to the
  // callback handling function).  Can be any pointer type, since it will
  // internally be passed as a void pointer.
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, storehere);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  //curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64; rv:58.0) Gecko/20100101 Firefox/58.0");
  // Run our HTTP GET command, capture the HTTP response code, and clean up.
  curl_easy_perform(curl);
//std::cout << "res: " << res << '\n';
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
//  std::cout <<"this is the http_code: " << http_code << "\n";
//  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  if (http_code == 200) {
//std::cout << "http_code: " << http_code << '\n';
    std::cout << "\nSuccessful response from " << url << std::endl;
  }
  else {
    std::cout << "Images not found: " << http_code << '\n';
  }
  return 0;
}

int main(int argc ,char * argv[]) {
  using namespace std::chrono;
  std::string image_file_name;
  std::string image_file_date;
  std::vector<std::string> d_vec;
  if (!check_usage(argc, argv, image_file_name, image_file_date)) usage(argc, argv);
//  std::cout << "image_file_name: " << image_file_name << ' ' << "image_file_date: "
//            << image_file_date << '\n';
  if (image_file_name.length() > 0) {
    std::ifstream image_file(image_file_name);
    if (image_file) {
      char line[100];
      while (image_file.getline(line, sizeof(line), '\n')) {
        d_vec.push_back(line);
        std::cout << "process file\n";
      }
    }
    else std::cout << "do nothing -- no dates in file\n";
  }
  if (image_file_date.length() > 0) {
    std::cout << "process date\n";
    d_vec.push_back(image_file_date);
  }
  //if ((image_file_name.length() == 0) && (image_file_date.length() == 0))
  //  std::cout << "do nothing\n";
  //json_data = get_rover_info_for_date(string date)
  //retrieve_image(json_data)
  for (const auto& v : d_vec) {
    std::vector<std::string> iv;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    iv = retrieve_image_info(v);
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    for (const auto& i : iv) retrieve_image(i);
    high_resolution_clock::time_point t3 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    duration<double> time_span1 = duration_cast<duration<double>>(t3 - t2);
    std::cout << "\nIt took " << time_span.count() << " seconds to obtain the image infomation from the web service.";
    std::cout << "\nIt took " << time_span1.count() << " seconds to get all of the images and save them to disk.\ndone!\n";
  }
  return 0;
}

