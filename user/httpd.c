#include <inc/lib.h>
#include <lwip/sockets.h>
#include <lwip/inet.h>

#define PORT 80
#define VERSION "0.1"
#define HTTP_VERSION "1.0"

#define E_BAD_REQ	1000

#define BUFFSIZE 512
#define MAXPENDING 5	// Max connection requests

#define STRINGSIZE 2048

struct http_request {
	int sock;
	char *url;
	char *version;
};

struct responce_header {
	int code;
	char *header;
};

struct responce_header headers[] = {
	{ 200, 	"HTTP/" HTTP_VERSION " 200 OK\r\n"
		"Server: jhttpd/" VERSION "\r\n"},
	{0, 0},
};

struct error_messages {
	int code;
	char *msg;
};

struct error_messages errors[] = {
	{400, "Bad Request"},
	{404, "Not Found"},
};

void wait_ms(int time)
{
  int curtime=sys_time_msec();
  while ((sys_time_msec()-curtime)<time)
  {
    sys_yield();
  }
}

static void
die(char *m)
{
	cprintf("%s\n", m);
	exit();
}

static void
req_free(struct http_request *req)
{
	free(req->url);
	free(req->version);
}

static int
send_header(struct http_request *req, int code)
{
	struct responce_header *h = headers;
	while (h->code != 0 && h->header!= 0) {
		if (h->code == code)
			break;
		h++;
	}

	if (h->code == 0)
		return -1;

	int len = strlen(h->header);
	if (write(req->sock, h->header, len) != len) {
    printf("length %d", len);
		die("Failed to send bytes to client");
	}

	return 0;
}

static int
send_data(struct http_request *req, char* buf, int len)
{
	// LAB 6: Your code here.
  //printf("sending %s\n", buf);
  int bytes=0;
//  while (bytes<len)
 // {
    bytes+=write(req->sock, &buf[bytes], len-bytes);
  //  wait_ms(10);
 // }
  return bytes;
}

static int
send_size(struct http_request *req, off_t size)
{
	char buf[64];
	int r;

	r = snprintf(buf, 64, "Content-Length: %ld\r\n", (long)size);
	if (r > 63)
		panic("buffer too small!");

	if (write(req->sock, buf, r) != r)
		return -1;

	return 0;
}

static const char*
mime_type(const char *file)
{
	//TODO: for now only a single mime type
	return "text/html";
}

static int
send_content_type(struct http_request *req)
{
	char buf[128];
	int r;
	const char *type;

	type = mime_type(req->url);
	if (!type)
		return -1;

	r = snprintf(buf, 128, "Content-Type: %s\r\n", type);
	if (r > 127)
		panic("buffer too small!");

	if (write(req->sock, buf, r) != r)
		return -1;

	return 0;
}

static int
send_header_fin(struct http_request *req)
{
	const char *fin = "\r\n";
	int fin_len = strlen(fin);

	if (write(req->sock, fin, fin_len) != fin_len)
		return -1;

	return 0;
}

// given a request, this function creates a struct http_request
static int
http_request_parse(struct http_request *req, char *request)
{
	const char *url;
	const char *version;
	int url_len, version_len;

	if (!req)
		return -1;

	if (strncmp(request, "GET ", 4) != 0)
		return -E_BAD_REQ;

	// skip GET
	request += 4;

	// get the url
	url = request;
	while (*request && *request != ' ') //skip to next whitespace
		request++;
	url_len = request - url;

	req->url = malloc(url_len + 1);
	memmove(req->url, url, url_len);
	req->url[url_len] = '\0';

	// skip space
	request++;

	version = request;
	while (*request && *request != '\n')
		request++;
	version_len = request - version;

	req->version = malloc(version_len + 1);
	memmove(req->version, version, version_len);
	req->version[version_len] = '\0';

	// no entity parsing

	return 0;
}

static int
send_error(struct http_request *req, int code)
{
	char buf[512];
	int r;

	struct error_messages *e = errors;
	while (e->code != 0 && e->msg != 0) {
		if (e->code == code)
			break;
		e++;
	}

	if (e->code == 0)
		return -1;

	r = snprintf(buf, 512, "HTTP/" HTTP_VERSION" %d %s\r\n"
			       "Server: jhttpd/" VERSION "\r\n"
			       "Connection: close"
			       "Content-type: text/html\r\n"
			       "\r\n"
			       "<html><body><p>%d - %s</p></body></html>\r\n",
			       e->code, e->msg, e->code, e->msg);

	if (write(req->sock, buf, r) != r)
		return -1;

	return 0;
}

static int
send_shtml(struct http_request *req, char* string, int len)
{
	char buf[512];
	int r;

	//r = snprintf(buf, 512, "<html><body><p>%250s</p></body></html>\r\n",string);

  write(req->sock, "<html><body><pre>", 17);
  int bytes=0;
  while (bytes<len)
  {
    bytes+=write(req->sock, &string[bytes], len-bytes);
    wait_ms(10);
  }
  write(req->sock, "</pre></body></html>\r\n", 22);
	//if (write(req->sock, buf, r) != r)
	//	return -1;

	return 0;
}

static int
parse_shtml(char* file, char* cmd)
{
  if (strncmp(file, "#<exec ", 7)!=0)
    return -1;
  file+=7; //skip to cmd
  char *startcmd=file;
  while (*file && *file != '>')
  {
    ++file;
  }
  memcpy(cmd, startcmd, file-startcmd);
  cmd[file-startcmd]='\0'; //NULL it
  return file-startcmd; //return length
}

void
runcmd(char* cmd, char* result, int result_len)
{
  //so make 2 pipes. an input and output
  //dup them to 0 and 1
  //write cmd into input
  //read result from output
  //strcat(cmd, "\r\n");
  int rdfile=open("/tmprd", O_CREAT|O_WRONLY|O_TRUNC);
  if (!rdfile)
  {
    printf("couldn't open temp file\n");
  }
  write(rdfile, cmd, strlen(cmd));
  write(rdfile, "\n", 1);
  close(rdfile);
  int wrfile=open("/tmpwr", O_CREAT|O_WRONLY|O_TRUNC);
  if (!wrfile)
  {
    printf("couldn't open temp file\n");
  }
  int pid=fork();
  if (pid==0)
  {
    close(1);
  dup(wrfile, 1);
  int id=0;
  id=spawnl("/sh", "sh", "/tmprd", (char*)0);
  if (id<0)
  {
	  printf("spawn sh: %e\n", id);
    exit();
  }
  memset(result,'\0',result_len);
  wait(id);
  close(wrfile);
  exit();
  }
  wait(pid);
  wrfile=open("/tmpwr", O_RDONLY);
  int bytes_read=0;
  bytes_read+=read(wrfile, result, result_len);
  close(wrfile);
  return;
}

static int
send_file(struct http_request *req)
{
	int r;
	off_t file_size = -1;
	int fd;

	// open the requested url for reading
	// if the file does not exist, send a 404 error using send_error
	// if the file is a directory, send a 404 error using send_error
	// set file_size to the size of the file

	// LAB 6: Your code here.
  struct Stat sbuf;
  int shtml=0;
  stat(req->url, &sbuf);
  fd=open(req->url, O_RDONLY);
  if(fd<0 || sbuf.st_isdir)
  {
    r=send_error(req, 404);
    return r;
  }
  char *cmd=malloc(20);
  char *string=malloc(STRINGSIZE);
  memset(string, '\0', STRINGSIZE);
  while (read(fd, &string[strlen(string)], STRINGSIZE-strlen(string))>0){};
  file_size = strlen(string);
  //now check if it's shtml
  int cmdlen=parse_shtml(string, cmd);
  if (cmdlen>0)
  {
    //printf("shtml parse match %s\n", cmd);
    runcmd(cmd, string, STRINGSIZE);
    //printf("result %s\n", string);
    file_size=strlen(string);
    shtml=1;
  }
  if (file_size==0)
  {
    send_error(req, 404);
    goto end;
  }
	if ((r = send_header(req, 200)) < 0)
		goto end;

	if ((r = send_size(req, file_size)) < 0)
		goto end;

	if ((r = send_content_type(req)) < 0)
		goto end;

	if ((r = send_header_fin(req)) < 0)
		goto end;

  if (shtml)
  {
    r=send_shtml(req, string, file_size);
    goto end;
  }
	r = send_data(req, string, file_size);
end:
  free(string);
  free(cmd);
	close(fd);
	return r;
}

static void
handle_client(int sock)
{
	struct http_request con_d;
	int r;
	char buffer[BUFFSIZE];
	int received = -1;
	struct http_request *req = &con_d;

	while (1)
	{
		// Receive message
		if ((received = read(sock, buffer, BUFFSIZE)) < 0)
			panic("failed to read");

		memset(req, 0, sizeof(req));

		req->sock = sock;

		r = http_request_parse(req, buffer);
		if (r == -E_BAD_REQ)
			send_error(req, 400);
		else if (r < 0)
			panic("parse failed");
		else
			send_file(req);

		req_free(req);

		// no keep alive
		break;
	}

	close(sock);
}

void
umain(int argc, char **argv)
{
	int serversock, clientsock;
	struct sockaddr_in server, client;

	binaryname = "jhttpd";

	// Create the TCP socket
	if ((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		die("Failed to create socket");

	// Construct the server sockaddr_in structure
	memset(&server, 0, sizeof(server));		// Clear struct
	server.sin_family = AF_INET;			// Internet/IP
	server.sin_addr.s_addr = htonl(INADDR_ANY);	// IP address
	server.sin_port = htons(PORT);			// server port

	// Bind the server socket
	if (bind(serversock, (struct sockaddr *) &server,
		 sizeof(server)) < 0)
	{
		die("Failed to bind the server socket");
	}

	// Listen on the server socket
	if (listen(serversock, MAXPENDING) < 0)
		die("Failed to listen on server socket");

	cprintf("Waiting for http connections...\n");

	while (1) {
		unsigned int clientlen = sizeof(client);
		// Wait for client connection
		if ((clientsock = accept(serversock,
					 (struct sockaddr *) &client,
					 &clientlen)) < 0)
		{
			die("Failed to accept client connection");
		}
		handle_client(clientsock);
	}

	close(serversock);
}
