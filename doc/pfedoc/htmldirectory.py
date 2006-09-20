#! /usr/bin/env python
# -*- coding: UTF-8 -*-
from match import Match
import os

class HtmlDirectory:
    """ binds some html content page with additional markup - in this
    base version it is just the header information while other variants
    might add navigation items around the content block elements """
    def __init__(self, o, directory = None):
        self.o = o
        self.directory = directory
        self.title = ""
        self.meta = []
        self.style = []
        self.text = []
        self.id = 100
        self.navi = None
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
    def get_filepart(self, text):
        try:   return text.get_filepart()
        except Exception, e: pass
        try:   return text.get_listname()
        except Exception, e: pass
        filename = str(self.id)
        self.id += 1
        return filename
    def get_title(self, text):
        if self.title: return self.title
        try:   return text.get_title()
        except Exception, e: pass
        return self.title
    def _html_meta(self, meta):
        """ accepts adapter objects with .html_meta() """
        try:   return meta.html_meta()
        except Exception, e: pass
        return str(meta)
    def _html_style(self, style):
        """ accepts adapter objects with .html_style() and .xml_style() """
        ee = None
        try:   return style.html_style()
        except Exception, e: ee = e; pass
        try:   return style.xml_style()
        except Exception, e: print "HtmlDirectory/style", ee, e; pass
        try:   return str(style)
        except Exception, e: print "HtmlDirectory/style", e; return ""
    def _html_text(self, html):
        """ accepts adapter objects with .html_text() and .xml_text() """
        ee = None
        try:   return html.html_text()
        except Exception, e: ee = e; pass
        try:   return html.xml_text()
        except Exception, e: print "HtmlDirectory/text", ee, e; pass
        try:   return str(html)
        except Exception, e: print "HtmlDirectory/text", e; return "&nbsp;"
    def navigation(self):
        if self.navi:
            return self.navi
        if self.o.body:
            try:
                fd = open(self.o.body, "r")
                self.navi = fd.read()
                fd.close()
                return self.navi
            except Exception, e:
                pass
        return None
    def html_header(self, text):
        navi = self.navigation()
        if not navi:
            T = "<html><head>"
            title = self.get_title(text)
            if title:
                T += "<title>"+title+"</title>"
            T += "\n"
            for style in self.style:
                T += self._html_style(style)
                T += "\n"
            return T+"</head><body>"
        else:
            title = self.get_title(text)
            return navi & (
                Match(r"((?:src|href)=\"(?!http:))") >> "\\1../") & (
                Match(r"<!--title-->") >> " - "+title) & (
                Match(r"<!--VERSION-->") >> self.o.version) & (
                Match(r"(?m).*</body></html>") >> "")
    def html_footer(self, text):
        navi = self.navigation()
        if not navi:
            return "</body></html>"
        else:
            return navi & (
                Match(r"(?m)(.*</body></html>)") >> "%&%&%&%\\1") & (
                Match(r"(?s).*%&%&%&%") >> "")
    def _filename(self, filename):
        if not filename & Match(r"\.\w+$"):
            ext = self.o.html
            if not ext: ext = "html"
            filename += "."+ext
        return filename
    def _dir_filename(self, directory, filename):
        if directory is not None:
            self.directory = directory
        directory = self.directory
        return self._filename(directory +"/"+ filename)
    def save(self, directory = None):
        if directory is None:
            directory = "html"
        try:
            print "mkdir '"+directory+"'"
            os.mkdir(directory)
        except Exception, e:
            print e
        index_text = "<ul>\n"
        for text in self.text:
            filepart = self.get_filepart(text)
            if self.save_file(text, directory, filepart):
                index_text += ('<li><a href="'+self._filename(filepart)+'">'+
                               filepart+'</li>'+"\n")
            else:
                index_text += "<!-- skipped "+filepart+" -->\n"
        index_text += "</ul>\n"
        self.save_file(index_text, directory, "index")
    def save_file(self, text, directory = None, filename = None):
        filename = self._dir_filename(directory, filename)
        text_body = self._html_text(text)
        if not text_body:
            print "# empty '"+filename+"'"
            return False
        print "writing '"+filename+"'"
        try:
            fd = open(filename, "w")
            print >>fd, self.html_header(text)
            print >>fd, text_body
            print >>fd, self.html_footer(text)
            fd.close()
            return True
        except IOError, e:
            print "could not open '"+filename+"'file", e
            return False
