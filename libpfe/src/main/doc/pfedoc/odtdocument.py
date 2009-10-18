#! /usr/bin/env python
# -*- coding: UTF-8 -*-
from match import Match
from zipfile import ZipFile, ZIP_DEFLATED
from odttypes import styles

manifest_xml = '''<?xml version="1.0" encoding="UTF-8"?>
<manifest:manifest xmlns:manifest="urn:oasis:names:tc:opendocument:xmlns:manifest:1.0">
 <manifest:file-entry manifest:media-type="application/vnd.oasis.opendocument.text" manifest:full-path="/"/>
 <manifest:file-entry manifest:media-type="text/xml" manifest:full-path="content.xml"/>
 <manifest:file-entry manifest:media-type="text/xml" manifest:full-path="styles.xml"/>
</manifest:manifest>'''

mimetype_txt = '''application/vnd.oasis.opendocument.text'''

def _odt_fixup(text):
    T = text & Match(r"(<function>[^<>]+)\\&lt\\;/link>") >> "\\1</function>"
    if T != text:
        pass # print "HACK FIXUP:", text
    T = T.replace("<para>", "<text:p text:style-name=\"Text_20_body\">")
    T = T.replace("</para>", "</text:p>")
    T = T.replace("<emphasis>", "<text:span text:style-name=\"Emphasis\">")
    T = T.replace("</emphasis>", "</text:span>")
    T = T.replace("<literal>", "<text:span text:style-name=\"Source_20_Text\">")
    T = T.replace("</literal>", "</text:span>")
    T = T.replace("<refentry>", "")
    T = T.replace("</refentry>", "")
    T = T & Match("(?s)<refentryinfo>.*?</refentryinfo>") >> ""
    T = T & Match("(?s)<refmeta>.*?</refmeta>") >> ""
    T = T.replace("<refnamediv>", "")
    T = T.replace("</refnamediv>", "")
    T = T.replace("<refname>", "<text:p text:style-name=\"Preformatted_20_Text\">")
    T = T.replace("</refname>", "</text:p>")
    T = T.replace("<refpurpose>", "<text:h text:style-name=\"Heading_20_3\">")
    T = T.replace("</refpurpose>", "</text:h>")
    T = T.replace("<refsynopsisdiv>", "")
    T = T.replace("</refsynopsisdiv>", "")
    T = T.replace("<funcsynopsisinfo>", "<text:p text:style-name=\"Preformatted_20_Text\">")
    T = T.replace("</funcsynopsisinfo>", "</text:p>")
    T = T.replace("<funcsynopsis>", "")
    T = T.replace("</funcsynopsis>", "")
    T = T.replace("<funcprototype>", "<text:p text:style-name=\"Preformatted_20_Text\">")
    T = T.replace("</funcprototype>", "</text:p>")
    T = T.replace("<funcdef>", "<text:span text:style-name=\"Source_20_Text\">")
    T = T.replace("</funcdef>", "</text:span>")
    T = T.replace("<paramdef>", "<text:span text:style-name=\"Source_20_Text\">")
    T = T.replace("</paramdef>", "</text:span>")
    T = T & Match("(?s)<refsect1><title>(.*?)</title>") \
        >> "<text:h text:style-name=\"Heading_20_2\">\\1</text:h>"
    T = T.replace("</refsect1>", "")
    T = T.replace("<function>", "<text:span text:style-name=\"Source_20_Text\">")
    T = T.replace("</function>", "</text:span>")
    T = T.replace("<itemizedlist>", "<text:ordered-list>")
    T = T.replace("</itemizedlist>", "</text:ordered-list>")
    T = T.replace("<listitem>", "<text:list-item>")
    T = T.replace("</listitem>", "</text:list-item>")
    T = T.replace("&nbsp;", "<text:s/>")
    T = T.replace("\\&lt\\;/code>", "</text:span>") # ???
    return T

class OdtDocument:
    """ binds some xml content page with additional markup """
    _xmlns = {
	"office" : "urn:oasis:names:tc:opendocument:xmlns:office:1.0",
	"style" : "urn:oasis:names:tc:opendocument:xmlns:style:1.0",
	"text" : "urn:oasis:names:tc:opendocument:xmlns:text:1.0" }
    def __init__(self, o, filename = None):
        self.o = o
        self.rootnode = "office:document-content"
        self.filename = filename
        self.title = ""
        self.text = []
    def add(self, text):
        """ add some content """
        self.text += [ text ]
        return self
    def get_title(self):
        if self.title: return title
        try:   return self.text[0].get_title()
        except Exception, e: pass
        return self.title
    def _xml_text(self, xml):
        """ accepts adapter objects with .xml_text() """
        try:   return _odt_fixup(xml.xml_text())
        except Exception, e: print "OdtDocument/text", e; pass
        return str(xml)
    def _filename(self, filename):
        if filename is not None:
            self.filename = filename
        filename = self.filename
        if not filename & Match(r"\.\w+$"):
            ext = self.o.ooxml
            if not ext: ext = "ooxml"
            filename += "."+ext
        return filename
    def _zipfilename(self, filename):
        if filename is not None:
            self.filename = filename
        filename = self.filename
        if not filename & Match(r"\.\w+$"):
            ext = self.o.odt
            if not ext: ext = "odt"
            filename += "."+ext
        return filename
    def save(self, name = None):
        filename = self._filename(name)
        print "writing '"+filename+"'"
        self.save_all(filename)
        zipfilename = self._zipfilename(name)
        print "writing '"+zipfilename+"'"
        self.save_zip(filename, zipfilename)
    def save_all(self, filename):
        try:
            fd = open(filename, "w")
            title = self.get_title()
            print >>fd, "<"+self.rootnode,
            for key,val in self._xmlns.items():
                print >> fd, " xmlns:"+key+'="'+val+'"',
            print >> fd, ">",
            print >> fd, "<office:scripts/>",
            # print >> fd, "<office:font-face-decls/>",
            # print >> fd, "<office:automatic-styles/>",
            print >> fd, "<office:body>",
            # text:h text:p text:ordered-list text:unordered-list
            # text:section
            if title:
                print >> fd, "<text:h><text:title>"+title,
                print >> fd, "</text:title></text:h>",
            for text in self.text:
                text = self._xml_text(text)
                print >>fd, text
            print >>fd, "</office:body>",
            print >>fd, "</"+self.rootnode+">"
            fd.close()
            return True
        except IOError, e:
            print "could not open '"+filename+"'file", e
            return False
    def save_zip(self, filename, zipfilename):
        zip = ZipFile(zipfilename, "w", ZIP_DEFLATED)
        zip.write(filename, "content.xml")
        zip.writestr("META-INF/manifest.xml", manifest_xml)
        zip.writestr("mimetype", mimetype_txt)
        zip.writestr("styles.xml", styles)
        zip.close()
        
        
