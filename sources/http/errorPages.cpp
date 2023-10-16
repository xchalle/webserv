#include <string>
#include "../../include/utils.hpp"

static std::string	getHeadPart(std::string title, std::string head_content)
{
	return ("<!DOCTYPE html><html><head><title>" + title + " error</title>" + head_content + "</head><body>");
}

static std::string getBottomPart(void)
{
	return ("</body></html>");
}

static std::string	defaultErrorPage(unsigned int code, const std::string& message)
{
	std::string str_code = to_string(code);
	std::string head = getHeadPart(str_code, "<style id='' media='all'>/* cyrillic-ext */ @font-face { font-family: 'Montserrat'; font-style: normal; font-weight: 900; font-display: swap; src: url(/fonts.gstatic.com/s/montserrat/v25/JTUHjIg1_i6t8kCHKm4532VJOt5-QNFgpCvC73w0aXpsog.woff2) format('woff2'); unicode-range: U+0460-052F, U+1C80-1C88, U+20B4, U+2DE0-2DFF, U+A640-A69F, U+FE2E-FE2F; } /* cyrillic */ @font-face { font-family: 'Montserrat'; font-style: normal; font-weight: 900; font-display: swap; src: url(/fonts.gstatic.com/s/montserrat/v25/JTUHjIg1_i6t8kCHKm4532VJOt5-QNFgpCvC73w9aXpsog.woff2) format('woff2'); unicode-range: U+0301, U+0400-045F, U+0490-0491, U+04B0-04B1, U+2116; } /* vietnamese */ @font-face { font-family: 'Montserrat'; font-style: normal; font-weight: 900; font-display: swap; src: url(/fonts.gstatic.com/s/montserrat/v25/JTUHjIg1_i6t8kCHKm4532VJOt5-QNFgpCvC73w2aXpsog.woff2) format('woff2'); unicode-range: U+0102-0103, U+0110-0111, U+0128-0129, U+0168-0169, U+01A0-01A1, U+01AF-01B0, U+1EA0-1EF9, U+20AB; } /* latin-ext */ @font-face { font-family: 'Montserrat'; font-style: normal; font-weight: 900; font-display: swap; src: url(/fonts.gstatic.com/s/montserrat/v25/JTUHjIg1_i6t8kCHKm4532VJOt5-QNFgpCvC73w3aXpsog.woff2) format('woff2'); unicode-range: U+0100-024F, U+0259, U+1E00-1EFF, U+2020, U+20A0-20AB, U+20AD-20CF, U+2113, U+2C60-2C7F, U+A720-A7FF; } /* latin */  </style> <style id='' media='all'>/* vietnamese */ @font-face { font-family: 'Cabin'; font-style: normal; font-weight: 400; font-stretch: 100%; font-display: swap; src: url(/fonts.gstatic.com/s/cabin/v26/u-4i0qWljRw-PfU81xCKCpdpbgZJl6XvptnsBXw.woff2) format('woff2'); unicode-range: U+0102-0103, U+0110-0111, U+0128-0129, U+0168-0169, U+01A0-01A1, U+01AF-01B0, U+1EA0-1EF9, U+20AB; } /* latin-ext */ @font-face { font-family: 'Cabin'; font-style: normal; font-weight: 400; font-stretch: 100%; font-display: swap; src: url(/fonts.gstatic.com/s/cabin/v26/u-4i0qWljRw-PfU81xCKCpdpbgZJl6Xvp9nsBXw.woff2) format('woff2'); unicode-range: U+0100-024F, U+0259, U+1E00-1EFF, U+2020, U+20A0-20AB, U+20AD-20CF, U+2113, U+2C60-2C7F, U+A720-A7FF; } /* vietnamese */ @font-face { font-family: 'Cabin'; font-style: normal; font-weight: 700; font-stretch: 100%; font-display: swap; src: url(/fonts.gstatic.com/s/cabin/v26/u-4i0qWljRw-PfU81xCKCpdpbgZJl6XvptnsBXw.woff2) format('woff2'); unicode-range: U+0102-0103, U+0110-0111, U+0128-0129, U+0168-0169, U+01A0-01A1, U+01AF-01B0, U+1EA0-1EF9, U+20AB; } /* latin-ext */ @font-face { font-family: 'Cabin'; font-style: normal; font-weight: 700; font-stretch: 100%; font-display: swap; src: url(/fonts.gstatic.com/s/cabin/v26/u-4i0qWljRw-PfU81xCKCpdpbgZJl6Xvp9nsBXw.woff2) format('woff2'); unicode-range: U+0100-024F, U+0259, U+1E00-1EFF, U+2020, U+20A0-20AB, U+20AD-20CF, U+2113, U+2C60-2C7F, U+A720-A7FF; } </style><style>*{-webkit-box-sizing:border-box;box-sizing:border-box}body{padding:0;margin:0}#notfound{position:relative;height:100vh;background: #22272e;}#notfound .notfound{position:absolute;left:50%;top:50%;-webkit-transform:translate(-50%,-50%);-ms-transform:translate(-50%,-50%);transform:translate(-50%,-50%)}.notfound{max-width:520px;width:100%;line-height:1.4;text-align:center}.notfound .notfound-404{position:relative;height:240px}.notfound .notfound-404 h1{font-family:montserrat,sans-serif;position:absolute;left:50%;top:50%;-webkit-transform:translate(-50%,-50%);-ms-transform:translate(-50%,-50%);transform:translate(-50%,-50%);font-size:252px;font-weight:900;margin:0;color:#ffffff;text-transform:uppercase;letter-spacing:-40px;margin-left:-20px}.notfound .notfound-404 h1>span{text-shadow:-8px 0 0 #161335}.notfound .notfound-404 h3{font-family:cabin,sans-serif;position:relative;font-size:16px;font-weight:700;text-transform:uppercase;color:#fff;margin:0;letter-spacing:3px;padding-left:6px}.notfound h2{font-family:cabin,sans-serif;font-size:20px;font-weight:400;text-transform:uppercase;color:#fff;margin-top:0;margin-bottom:25px}@media only screen and (max-width:767px){.notfound .notfound-404{height:200px}.notfound .notfound-404 h1{font-size:200px}}@media only screen and (max-width:480px){.notfound .notfound-404{height:162px}.notfound .notfound-404 h1{font-size:162px;height:150px;line-height:162px}.notfound h2{font-size:16px}}</style>");
	return (head + "<div id='notfound'><div class='notfound'><div class='notfound-404'><h3>Oops! "+message+"</h3><h1><span>"+str_code[0]+"</span><span>"+str_code[1]+"</span><span>"+str_code[2]+"</span></h1></div><h2>An error has occurred, "+message+"</h2></div></div>" + getBottomPart());
}

std::string getErrorPage(unsigned int code, const std::string& message)
{
	if (code < 100 || code > 999)
		return ("");
	switch (code)
	{
		default:
			return (defaultErrorPage(code, message));
	}
}