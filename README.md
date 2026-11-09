# dWebServer (Name Pending)

<img src="https://raw.githubusercontent.com/DASPELLER4/dWebServer/refs/heads/assets/logo.png" height=200px>

A simple webserver written in C for creating websites in C, if you want to try the example out, run

```
make clean; make ENDPOINTS=examples/standard; ./server
```
And open a browser to `http://localhost:3000/`

This example has a basic HTML response on `/`, a POST `multipart/form-data` endpoint on `/formsubmit`, with binary file reading on `/favicon.ico`. It serves as a demo of an extremely basic forum/user comments feature.

The webserver itself has _enough_ features to allow websites to be created, however features like rate-limits, max request sizes and basic code-quality/safety have not _yet_ been implemented. Hence, for now, the server is intended to be modified directly on `main.c` for anything secure/stable.

## Prerequesites
This webserver requires:
|Package|Version|
|-------|-------|
|GCC|>10.3|
|GNU/Linux|>3.1|
|GNU Make|>4.0|
|glibc|>2.4|
|_Python*_|>3.0|

_* only required for generating `example/automaticstaticpage`, although this does come pregenerated_
## Making a site

To make a website using Name Pending, you only need to write an `endpoints.h` file and `endpoints.c` file.

`endpoints.h` should define the functions that run when a specific resource is accessed, these functions take in `request_t` and should return `response_t`, for example, the demo uses
```
pushEndpoints
#undef endpoints
#define endpoints popEndpoints endpoints addEndpoint(root, "/");
response_t root(request_t);
```

Where the first 2 lines are just boilerplate macros that you must use unmodified, and the third line specifies the function name to be called (`root`), and the URL that calls it (`/`). The last line is the actual function declaration.

If you want to make a page that loads for a format of URL (similar to wildcards in Node.JS), you have to use the 404 page feature, and parse the request.path yourself. To define a "404" page (does not necessarily have to return 404 as a status, but can just be an arbitrary page of any status code), you have to define the `is404` macro and define the function `response_t fourohfour(request_t)`. For example:
```
#define is404
response_t fourohfour(request_t);
```
### Example endpoint
An example definition for an endpoint is given below:
```
response_t fourohfour(request_t request){
        header_t *headers = newHeaders();
        appendHeader(headers, (keyvalue_t){0,"Content-Type","text/html"});
        static char *body = "<!DOCTYPE html><body><h1>404</h1><p>No such page found!!</p></body>";
        int len = strlen(body);
        static char d[120] = {0};
        sprintf(d, "%d", len);
        appendHeader(headers, (keyvalue_t){0,"Content-Length", d});
        return (response_t){404, "Not Found", headers, body, len+1};
}
```
This code is very basic, it first specifies the headers for the response, and appends a header specifying the `Content-Type`.

It then defines the body of the response, note the use of `static` here, this is because the string must stay in scope once the endpoint function has ended.

Then it determines the `Content-Length` for the body _as a string_, since headers only work on strings.

It then creates the `response_t` struct, that contains the status code, reason phrase, headers, body, and the total length of the body (this length INCLUDES any null terminators and **must** be correct for the body being returned).
### Running your site
Using make, you can compile your site inside the `dWebServer` folder using
```
make ENDPOINTS=endpointfolder PORT=3000 DEBUG=1
```
Where `ENDPOINTS` is the folder containing `endpoints.c` and `endpoints.h`, `PORT` specifies the port (default 80 if unspecified) and `DEBUG` defines the `DEBUG` macro, which produces debug info during runtime.

(note that if you are changing the value of `ENDPOINTS`, you may need to run `make clean` before compiling, since the makefile may not notice that you changed it.)

To run your server, just run 
```
./server
```
If you used a privileged port such as 80, you may need to run this command as root (not really recommended in this current stage of development).

## Examples
There are currently two examples:
### `examples/standard`
This example shows off almost all of the features of the webserver, with two dynamic pages, one that keeps memory of all requests to the second, and one that parses and saves the inputs to the page. It also has a favicon that is loaded as a file.

This one is reasonably complex, since it parses body data and headers, but is still _reasonably_ understandable.
### `examples/automaticstaticpage`
This is an actually useful example that uses a python script and the current directory structure to automatically generate `endpoints.c` and `endpoints.h`, this allows you to generate static pages very quickly that also load very quickly on the server side.

## HTTPS
HTTPS is not currently supported, this is planned with OpenSSL, but for now, I recommend you use NGINX as a reverse proxy for HTTPS traffic.

## All Features
### Macros
Some of these macros (indicated with a *) can be specified in your make command, but these are intended to be set in `userconfig.h` within your endpoint folder.
|Name|Description|Default|
|----|-----------|-------|
|PORT*|Specifies the port the HTTP server runs on|80|
|ENDPOINTHASHSIZE*|Specifies the size of the endpoint array, if this value is too small you may encounter endpoint hash collisions|256|
|PREDATASIZE|Specifies the maximal size of a request before the body (only type of request, path, version and headers are included here)|1000|
|MAXTIME|Specifies, in seconds, the maximal total time receiving an entire request; otherwise it will error out to the client|10|
|TIMEOUT|Specifies, in seconds, the maximal time the server can spend on a single read(); otherwise it will error out to the client|10|
|MAXMEMORYUSE|Specifies, in Bytes, how large the request can be before it gets processed as a file and not in memory|10000000|
|DEBUG*|Output debug text during runtime||
|STATIC|Completely skips all request processing past the first line of the request, this is very fast, but it prevents your website from knowing any request headers or any of the request body||

### Parsers
Currently, only one body parser is included, this is in "bodyparser.h" and includes:
* `parsedBody_t parseMultiFormData(request_t)`
* `parsedData_t *getFieldByName(parsedBody_t, char*)`

Where the first takes in the request passed into your endpoint and returns a dynamic array of `parsedData_t` (`.data`) and size `.count` for requests of type `multipart/form-data`.

The second returns a parsedBody_t struct for a field of given name.

`parsedData_t` is a struct consisting of:
* `.count`: Size of the dynamic array containing the field parameters
* `.params`: Dynamic array containing `keyvalue_t` that have a parameter (`.key`) and it's value (`.value`), for example: `.key = "Content-Disposition"`, `.value = "form-data"` or `.key = "filename"`, `.value = "test.mp4"`. This also includes the name of the field as `.value = "name"`
* `.value`: A `char*` of the value for that field
* `.length`: Length of the above character array
