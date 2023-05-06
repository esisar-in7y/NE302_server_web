**METHOD**
METHOD = [GET,HEAD,POST] if not in METHOD => 501 Not Implemented

**HTTP-VERSION**
If version not in [0.9,1.0,1.1,2.0,3.0] (exist) => 400 Bad Request

If version not in [0.9,1.0,1.1] (supported)  => 505 HTTP Version Not Supported

If version 1.1 and no Host header => 400 Bad Request

**Request-target**
Gérer le % encoding et le dot segment removal
## A finir de détailler

**Transfer-Encoding-header**

Verify that the message body isn't chunked more than once

Verify that the message body is chunked after the use of another transfer coding (not chunk)

If it's a transfer-coding it doesn't understand => 501 Not Implemented

If 1.0 and Transfer-Encoding header => 400 Bad Request

**Cookie-header**

