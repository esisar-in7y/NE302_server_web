**METHOD**
METHOD = [GET,HEAD,POST] if not in METHOD => 501 Not Implemented

**HTTP-VERSION**
If version not in [0.9,1.0,1.1,2.0,3.0] (exist) => 400 Bad Request

If version not in [0.9,1.0,1.1] (supported)  => 505 HTTP Version Not Supported



**Request-target**
Gérer le % encoding et le dot segment removal

origin-form
   The most common form of request-target is the origin-form.
     origin-form    = absolute-path [ "?" query ]
	 
absolute-form
   When making a request to a proxy, a client MUST send the target
   URI in absolute-form as the request-target.
     absolute-form  = absolute-URI
	 
	 
**Transfer-Encoding-header**

Verify that the message body isn't chunked more than once

Verify that the message body is chunked after the use of another transfer coding (not chunk)

If it's a transfer-coding it doesn't understand => 501 Not Implemented

If 1.0 and Transfer-Encoding header => 400 Bad Request

**Cookie-header**
Je sais pas

**Referer header**

If the URL contains a fragment (indicated by a '#' symbol) or userinfo (indicated by a '@' symbol) => 400 Bad Request

**User-Agent header**

Je sais pas

**Accept header**


Verif quality is between 0 and 1 with at most 3 decimals after .
Verif types/subtypes (peut être)


**Accept-encoding header**

Verif one of the content codings listed is the representation's content coding (si q!=0)  or \*/\*else =>  415 Unsupported Media Type
Without Accept-encoding => everything is considered as acceptable so do nothing
If representation has no content coding => acceptable 

**Content-Length header**
max
(Value=>0) => (Value<max) => OK sinon 413 Content Too Large sinon 400 Bad Request

Content-Length and Transfer-encoding can't be in the same request

**Host header**

Overlap with Request-target for URI

If version 1.1 and no Host header => 400 Bad Request

If request-target include authority component and Host have a value different to this component => 400 Bad Request

If several Host header => 400 Bad Request

**Connection header**

If Version 1.0 and keep Alive option, must have Transfer-Encoding or Content-Length, if not 400 Bad Request

If Version 1.1 and no close option, must have Transfer-Encoding or Content-Length, if not 400 Bad Request

If is a proxy and Version 1.0 and keep Alive option or Version 1.1 and no close option => 400 Bad Request








