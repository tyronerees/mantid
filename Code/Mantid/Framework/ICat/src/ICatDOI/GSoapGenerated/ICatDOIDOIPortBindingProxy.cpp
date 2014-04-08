/* ICatDOIDOIPortBindingProxy.cpp
   Generated by gSOAP 2.8.15 from ICatDOI.h

Copyright(C) 2000-2013, Robert van Engelen, Genivia Inc. All Rights Reserved.
The generated code is released under ONE of the following licenses:
GPL or Genivia's license for commercial use.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
*/

#include "MantidICat/ICatDOI/GSoapGenerated/ICatDOIDOIPortBindingProxy.h"

namespace ICatDOI {

DOIPortBindingProxy::DOIPortBindingProxy()
{	DOIPortBindingProxy_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

DOIPortBindingProxy::DOIPortBindingProxy(const struct soap &_soap) : soap(_soap)
{ }

DOIPortBindingProxy::DOIPortBindingProxy(const char *url)
{	DOIPortBindingProxy_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
	soap_endpoint = url;
}

DOIPortBindingProxy::DOIPortBindingProxy(soap_mode iomode)
{	DOIPortBindingProxy_init(iomode, iomode);
}

DOIPortBindingProxy::DOIPortBindingProxy(const char *url, soap_mode iomode)
{	DOIPortBindingProxy_init(iomode, iomode);
	soap_endpoint = url;
}

DOIPortBindingProxy::DOIPortBindingProxy(soap_mode imode, soap_mode omode)
{	DOIPortBindingProxy_init(imode, omode);
}

DOIPortBindingProxy::~DOIPortBindingProxy()
{ }

void DOIPortBindingProxy::DOIPortBindingProxy_init(soap_mode imode, soap_mode omode)
{	soap_imode(this, imode);
	soap_omode(this, omode);
	soap_endpoint = NULL;
	static const struct Namespace namespaces[] =
{
	{"SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/", "http://www.w3.org/*/soap-envelope", NULL},
	{"SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/", "http://www.w3.org/*/soap-encoding", NULL},
	{"xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL},
	{"xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL},
	{"ns1", "http://webservice.doi.stfc.ac.uk/", NULL, NULL},
	{NULL, NULL, NULL, NULL}
};
	soap_set_namespaces(this, namespaces);
}

void DOIPortBindingProxy::destroy()
{	soap_destroy(this);
	soap_end(this);
}

void DOIPortBindingProxy::reset()
{	destroy();
	soap_done(this);
	soap_init(this);
	DOIPortBindingProxy_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

void DOIPortBindingProxy::soap_noheader()
{	this->header = NULL;
}

const SOAP_ENV__Header *DOIPortBindingProxy::soap_header()
{	return this->header;
}

const SOAP_ENV__Fault *DOIPortBindingProxy::soap_fault()
{	return this->fault;
}

const char *DOIPortBindingProxy::soap_fault_string()
{	return *soap_faultstring(this);
}

const char *DOIPortBindingProxy::soap_fault_detail()
{	return *soap_faultdetail(this);
}

int DOIPortBindingProxy::soap_close_socket()
{	return soap_closesock(this);
}

int DOIPortBindingProxy::soap_force_close_socket()
{	return soap_force_closesock(this);
}

void DOIPortBindingProxy::soap_print_fault(FILE *fd)
{	::soap_print_fault(this, fd);
}

#ifndef WITH_LEAN
#ifndef WITH_COMPAT
void DOIPortBindingProxy::soap_stream_fault(std::ostream& os)
{	::soap_stream_fault(this, os);
}
#endif

char *DOIPortBindingProxy::soap_sprint_fault(char *buf, size_t len)
{	return ::soap_sprint_fault(this, buf, len);
}
#endif

int DOIPortBindingProxy::registerInvestigationDOI(const char *endpoint, const char *soap_action, ns1__registerInvestigationDOI *ns1__registerInvestigationDOI_, ns1__registerInvestigationDOIResponse *ns1__registerInvestigationDOIResponse_)
{	struct soap *soap = this;
	struct __ns1__registerInvestigationDOI soap_tmp___ns1__registerInvestigationDOI;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "https://data4.isis.stfc.ac.uk:443/doi/DOIService";
	if (soap_action == NULL)
		soap_action = "http://webservice.doi.stfc.ac.uk/DOI/registerInvestigationDOIRequest";
	soap->encodingStyle = NULL;
	soap_tmp___ns1__registerInvestigationDOI.ns1__registerInvestigationDOI_ = ns1__registerInvestigationDOI_;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___ns1__registerInvestigationDOI(soap, &soap_tmp___ns1__registerInvestigationDOI);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___ns1__registerInvestigationDOI(soap, &soap_tmp___ns1__registerInvestigationDOI, "-ns1:registerInvestigationDOI", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___ns1__registerInvestigationDOI(soap, &soap_tmp___ns1__registerInvestigationDOI, "-ns1:registerInvestigationDOI", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!ns1__registerInvestigationDOIResponse_)
		return soap_closesock(soap);
	ns1__registerInvestigationDOIResponse_->soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	ns1__registerInvestigationDOIResponse_->soap_get(soap, "ns1:registerInvestigationDOIResponse", "ns1:registerInvestigationDOIResponse");
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int DOIPortBindingProxy::registerDatafileDOI(const char *endpoint, const char *soap_action, ns1__registerDatafileDOI *ns1__registerDatafileDOI_, ns1__registerDatafileDOIResponse *ns1__registerDatafileDOIResponse_)
{	struct soap *soap = this;
	struct __ns1__registerDatafileDOI soap_tmp___ns1__registerDatafileDOI;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "https://data4.isis.stfc.ac.uk:443/doi/DOIService";
	if (soap_action == NULL)
		soap_action = "http://webservice.doi.stfc.ac.uk/DOI/registerDatafileDOIRequest";
	soap->encodingStyle = NULL;
	soap_tmp___ns1__registerDatafileDOI.ns1__registerDatafileDOI_ = ns1__registerDatafileDOI_;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___ns1__registerDatafileDOI(soap, &soap_tmp___ns1__registerDatafileDOI);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___ns1__registerDatafileDOI(soap, &soap_tmp___ns1__registerDatafileDOI, "-ns1:registerDatafileDOI", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___ns1__registerDatafileDOI(soap, &soap_tmp___ns1__registerDatafileDOI, "-ns1:registerDatafileDOI", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!ns1__registerDatafileDOIResponse_)
		return soap_closesock(soap);
	ns1__registerDatafileDOIResponse_->soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	ns1__registerDatafileDOIResponse_->soap_get(soap, "ns1:registerDatafileDOIResponse", "ns1:registerDatafileDOIResponse");
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int DOIPortBindingProxy::registerDatasetDOI(const char *endpoint, const char *soap_action, ns1__registerDatasetDOI *ns1__registerDatasetDOI_, ns1__registerDatasetDOIResponse *ns1__registerDatasetDOIResponse_)
{	struct soap *soap = this;
	struct __ns1__registerDatasetDOI soap_tmp___ns1__registerDatasetDOI;
	if (endpoint)
		soap_endpoint = endpoint;
	if (soap_endpoint == NULL)
		soap_endpoint = "https://data4.isis.stfc.ac.uk:443/doi/DOIService";
	if (soap_action == NULL)
		soap_action = "http://webservice.doi.stfc.ac.uk/DOI/registerDatasetDOIRequest";
	soap->encodingStyle = NULL;
	soap_tmp___ns1__registerDatasetDOI.ns1__registerDatasetDOI_ = ns1__registerDatasetDOI_;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___ns1__registerDatasetDOI(soap, &soap_tmp___ns1__registerDatasetDOI);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___ns1__registerDatasetDOI(soap, &soap_tmp___ns1__registerDatasetDOI, "-ns1:registerDatasetDOI", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___ns1__registerDatasetDOI(soap, &soap_tmp___ns1__registerDatasetDOI, "-ns1:registerDatasetDOI", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!ns1__registerDatasetDOIResponse_)
		return soap_closesock(soap);
	ns1__registerDatasetDOIResponse_->soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	ns1__registerDatasetDOIResponse_->soap_get(soap, "ns1:registerDatasetDOIResponse", "ns1:registerDatasetDOIResponse");
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

} // namespace ICatDOI

/* End of client proxy code */