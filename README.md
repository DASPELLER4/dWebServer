# dWebServer (Name Pending)

<img src="https://raw.githubusercontent.com/DASPELLER4/dWebServer/refs/heads/assets/logo.png" height=200px>

A simple webserver written in C for creating websites in C, if you want to try the example out, run

```
make clean; make ENDPOINTS=examples/standard PORT=3000 DEBUG=1; ./server
```

This example has a basic HTML response on `/`, a POST `multipart/form-data` endpoint on `/formsubmit`, with binary file reading on `/favicon.ico`. It serves as a demo of an extremely basic forum/user comments feature.

The webserver itself has _enough_ features to allow websites to be created, however features like rate-limits, max request sizes and basic code-quality/safety have not _yet_ been implemented. Hence, for now, the server is intended to be modified directly on `main.c` for anything secure/stable.

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
