#include <assert.h>

#include "htmlWrapper.h"


void HtmlWrapper::begin_paragraph()
{
    _paragraph_level++;
    _inner_html += "<p>";
}

void HtmlWrapper::end_paragraph()
{
    _paragraph_level--;
    _inner_html += "</p>";
}

void HtmlWrapper::begin_list()
{
    _list_level++;
    _inner_html += "<ul>";
}

void HtmlWrapper::end_list()
{
    assert(_list_level > 0);
    _list_level--;
    _inner_html += "</ul>";
}

void HtmlWrapper::push_list_item(std::string text)
{
    assert(_list_level > 0);
    _inner_html += "<li>" + text + "</li>";
}

void HtmlWrapper::push_header(std::string text, int level)  // level should be from 1 to 6
{
    assert(level >= 1 && level <= 6);
    auto header_name = "h" + std::to_string(level);
    _inner_html += "<" + header_name + ">" +
            text +
            "</" + header_name + ">";
}

void HtmlWrapper::push_text(std::string text)
{
    _inner_html += text;
}

std::string HtmlWrapper::GenerateHtml()
{
    assert(_paragraph_level == 0);
    assert(_list_level == 0);

    std::string html = "<html><body>" +
            _inner_html +
            "</body></html>";
    return html;
}
