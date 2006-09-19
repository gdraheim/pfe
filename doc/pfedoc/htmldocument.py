#! /usr/bin/env python
# -*- coding: UTF-8 -*-

class HtmlDocument:
    """ binds some html content page with additional markup - in this
    base version it is just the header information while other variants
    might add navigation items around the content block elements """
    def __init__(self, filename = None):
        self.filename = filename
        self.title = ""
        self.meta = []
        self.style = []
        self.text = []
    def meta(self, style):
        """ add some header meta entry """
        self.meta += [ meta ]
        return self
    def style(self, style):
        """ add a style block """
        self.style += [ style ]
        return self
    def add(self, text):
        """ add some content """
        self.text += [ text ]
        return self
    def get_title(self):
        if self.title: return self.title
        try:   return self.text[0].get_title()
        except Exception, e: pass
        return self.title
    def _html_meta(self, meta):
        """ accepts adapter objects with .html_meta() """
        try:   return meta.html_meta()
        except Exception, e: pass
        return str(meta)
    def _html_style(self, style):
        """ accepts adapter objects with .html_style() and .xml_style() """
        try:   return style.html_style()
        except Exception, e: pass
        try:   return style.xml_style()
        except Exception, e: pass
        try:   return str(style)
        except Exception, e: print "_html_style():", e; return ""
    def _html_text(self, html):
        """ accepts adapter objects with .html_text() and .xml_text() """
        try:   return html.html_text()
        except Exception, e: print "_html_text(1):", e; pass
        try:   return html.xml_text()
        except Exception, e: print "_html_text(2):", e; pass
        try:   return str(html)
        except Exception, e: print "_html_text(3):", e; return "&nbsp;"
    def save(self, filename = None):
        if filename is not None:
            self.filename = filename
        filename = self.filename
        print "writing '"+filename+"'"
        try:
            fd = open(filename, "w")
            title = self.get_title()
            if title:
                print >>fd, "<html><head><title>"+title+"</title>"
            else:
                print >>fd, "<html><head>"
            for style in self.style:
                style = self._html_style(style)
                print >>fd, "<style>"+style+"</style>"
            print >>fd, "</head><body>"
            for text in self.text:
                text = self._html_text(text)
                print >>fd, text
            print >>fd, "</body></html>"
            fd.close()
            return True
        except IOError, e:
            print "could not open '"+filename+"'file", e
            return False
