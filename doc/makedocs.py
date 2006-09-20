#! /usr/bin/env python
# -*- coding: UTF-8 -*-
import sys
import os.path
from pfedoc.match import *
from pfedoc.options import *
from pfedoc.textfile import *
from pfedoc.textfileheader import *
from pfedoc.functionheader import *
from pfedoc.functionprototype import *
from pfedoc.commentmarkup import *
from pfedoc.functionlisthtmlpage import *
from pfedoc.functionlistreference import *
from pfedoc.forthheader import *
from pfedoc.forthnotation import *
from pfedoc.forthwordset import *
from pfedoc.forthwords import *
from pfedoc.forthwordsethtmlpage import *
from pfedoc.dbk2htm import *
from pfedoc.htmldocument import *
from pfedoc.htmldirectory import *
from pfedoc.docbookdocument import *

def _src_to_xml(text):
    return text.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
def _email_to_xml(text):
    return text & Match("<([^<>]*@[^<>]*)>") >> "&lt;\\1&gt;"
def _force_xml(text):
    if not text: return text
    text &= Match(r"<(/\w[\w:]*)>") >> "°[[[°\\1°]]]°"
    text &= Match(r"<(\w[\w:]*)>") >> "°[[[°\\1°]]]°"
    text &= Match(r"<(\w[\w:]*\s+\w+=[^<>]*)>") >> "°[[[°\\1°]]]°"
    text = _src_to_xml(text)
    return _hack_fixup(text.replace("°[[[°", "<").replace("°]]]°", ">"))
def _hack_fixup(text):
    return text & (
        Match("<(cr|closeparen|paren|quote|char|backtick|eol|eof|rp)>")
        >> "&lt;\\1;&gt;") & (
        Match("(>)<([ABN])>(</)") >> "\\1&lt;\\2&gt;\\3") & (
        Match("<br\s*/*>") >> "") & (
        Match("</?ansref>") >> "") & (
        Match("<small>((?:.(?!</small>))*.)</small>") >> "(\\1)")
def _link_code(text):
    return text.replace("<link>","<code>").replace("</link>","</code>")


class PerFileEntry:
    def __init__(self, header, comment):
        self.textfileheader = header
        self.filecomment = comment
class PerFile:
    def __init__(self):
        self.textfileheaders = []
        self.filecomments = []
        self.entries = []
    def add(self, textfileheader, filecomment):
        self.textfileheaders += [ textfileheader ]
        self.filecomments += [ filecomment ]
        self.entries += [ PerFileEntry(textfileheader, filecomment) ]
    def where_filename(self, filename):
        for entry in self.entries:
            if entry.textfileheader.get_filename() == filename:
                return entry
        return None
    def print_list_mainheader(self):
        for t_fileheader in self.textfileheaders:
            print t_fileheader.get_filename(), t_fileheader.src_mainheader()


class PerForthWordEntry:
    def __init__(self, header, comment, notation, word = None):
        self.header = header
        self.comment = comment
        self.notation = notation
        self.word = word
    def get_name(self):
        return self.notation.get_name()
    def get_titleline(self):
        return self.header.get_titleline()
    def get_head(self):
        return self.notation
    def get_body(self):
        return self.comment
    def get_word(self):
        return self.word
class PerForthWord:
    def __init__(self):
        self.headers = []
        self.comments = []
        self.notations = []
        self.entries = []
    def add(self, forthheader, forthcomment, forthnotation):
        self.headers += [ forthheader ]
        self.comments += [ forthnotation ]
        self.notations += [ forthnotation ]
        self.entries += [ PerForthWordEntry(
            forthheader, forthcomment, forthnotation) ]
    def where_name(self, name):
        for entry in self.entries:
            if entry.get_name() == name:
                return entry
        return None
    def print_list_titleline(self):
        for funcheader in self.headers:
            print funcheader.get_filename(), "[=>]", funcheader.get_titleline()
    def print_list_name(self):
        for funcheader in self.notations:
            print funcheader.get_filename(), "[>>]", funcheader.get_name()

class PerForthWordsetEntry:
    def __init__(self, wordset):
        self.wordset = wordset
    def get_name(self):
        return self.wordset.get_name()
class PerForthWordset:
    def __init__(self):
        self.entries = []
    def add(self, wordset):
        self.entries += [ PerForthWordsetEntry(wordset) ]

class PerFunctionEntry:
    def __init__(self, header, comment, prototype):
        self.header = header
        self.comment = comment
        self.prototype = prototype
    def get_name(self):
        return self.prototype.get_name()
    def get_titleline(self):
        return self.header.get_titleline()
    def get_head(self):
        return self.prototype
    def get_body(self):
        return self.comment
class PerFunction:
    def __init__(self):
        self.headers = []
        self.comments = []
        self.prototypes = []
        self.entries = []
    def add(self, functionheader, functioncomment, functionprototype):
        self.headers += [ functionheader ]
        self.comments += [ functionprototype ]
        self.prototypes += [ functionprototype ]
        self.entries += [ PerFunctionEntry(functionheader, functioncomment,
                                           functionprototype) ]
    def print_list_titleline(self):
        for funcheader in self.headers:
            print funcheader.get_filename(), "[=>]", funcheader.get_titleline()
    def print_list_name(self):
        for funcheader in self.prototypes:
            print funcheader.get_filename(), "[>>]", funcheader.get_name()

class PerFunctionFamilyEntry:
    def __init__(self, leader):
        self.leader = leader
        self.functions = []
    def contains(self, func):
        for item in self.functions:
            if item == func: return True
        return False
    def add(self, func):
        if not self.contains(func):
            self.functions += [ func ]
    def get_name(self):
        if self.leader is None: return None
        return self.leader.get_name()
class PerFunctionFamily:
    def __init__(self):
        self.functions = []
        self.families = []
        self.retarget = {}
        self.entries = []
    def add_PerFunction(self, per_list):
        for item in per_list.entries:
            add_PerFunctionEntry(item)
    def add_PerFunctionEntry(self, item):
        self.functions += [ item ]
    def get_function(self, name):
        for item in self.functions:
            if item.get_name() == name:
                return item
        return None
    def get_entry(self, name):
        for item in self.entries:
            if item.get_name() == name:
                return item
        return None
    def fill_families(self):
        name_list = {}
        for func in self.functions:
            name = func.get_name()
            name_list[name] = func
        for func in self.functions:
            name = func.get_name()
            line = func.get_titleline()
            is_retarget = Match("=>\s*(\w+)")
            if line & is_retarget:
                into = is_retarget[1]
                self.retarget[name] = is_retarget[1]
        lead_list = []
        for name in self.retarget:
            into = self.retarget[name]
            if into not in name_list:
                print ("function '"+name+"' retarget into '"+into+
                       "' does not exist - keep alone")
            if into in self.retarget:
                other = self.retarget[into]
                print ("function '"+name+"' retarget into '"+into+
                       "' which is itself a retarget into '"+other+"'")
            if into not in lead_list:
                lead_list += [ into ]
        for func in self.functions:
            name = func.get_name()
            if name not in lead_list and name not in self.retarget:
                lead_list += [ name ]
        for name in lead_list:
            func = self.get_function(name)
            if func is not None:
                entry = PerFunctionFamilyEntry(func)
                entry.add(func) # the first
                self.entries += [ entry ]
            else:
                print "head function '"+name+" has no entry"
        for func in self.functions:
            name = func.get_name()
            if name in self.retarget:
                into = self.retarget[name]
                entry = self.get_entry(into)
                if entry is not None:
                    entry.add(func) # will not add duplicates
                else:
                    print "into function '"+name+" has no entry"
    def print_list_name(self):
        for family in self.entries:
            name = family.get_name()
            print name, ":",
            for item in family.functions:
                print item.get_name(), ",",
            print ""
class HtmlManualPageAdapter:
    def __init__(self, entry):
        """ usually takes a PerFunctionEntry """
        self.entry = entry
    def get_name(self):
        return self.entry.get_name()
    def _head(self):
        return self.entry.get_head()
    def _body(self):
        return self.entry.get_body()
    def head_xml_text(self):
        return self._head().xml_text()
    def body_xml_text(self, name):
        return self._body().xml_text(name)
    def head_get_prespec(self):
        return self._head().get_prespec()
    def head_get_namespec(self):
        return self._head().get_namespec()
    def head_get_callspec(self):
        return self._head().get_callspec()
    def get_title(self):
        return self._body().header.get_title()
    def get_filename(self):
        return self._body().header.get_filename()
    def src_mainheader(self):
        return self._body().header.parent.textfile.src_mainheader()
    def get_mainheader(self):
        return _src_to_xml(self.src_mainheader())
    def is_fcode(self):
        return self._head().is_fcode()
class RefEntryManualPageAdapter:
    def __init__(self, entry, per_file = None):
        """ usually takes a PerFunctionEntry """
        self.entry = entry
        self.per_file = per_file
    def get_name(self):
        name = self.entry.get_name()
        if name: return _force_xml(name)
        return "??name??"
    def _head(self):
        return self.entry.get_head()
    def _body(self):
        return self.entry.get_body()
    def head_xml_text(self):
        return _force_xml(self._head().xml_text())
    def body_xml_text(self, name):
        return _force_xml(self._body().xml_text(name))
    def get_title(self):
        return _force_xml(self._body().header.get_title())
    def get_filename(self):
        return self._body().header.get_filename()
    def src_mainheader(self):
        mainheader = self._body().header.parent.textfile.src_mainheader()
        if not mainheader:
            filename_c = self.get_filename()
            filename_h = filename_c & Match("[.]c$") >> ".h"
            if filename_c != filename_h and os.path.exists(filename_h):
                headername = filename_h & Match("\\.\\./") >> ""
                mainheader = "#include <"+headername+">"
        return mainheader
    def get_mainheader(self):
        return _src_to_xml(self.src_mainheader())
    def get_includes(self):
        return ""
    def list_seealso(self):
        return self._body().header.get_alsolist()
    def is_fcode(self):
        return self._head().is_fcode()
    def get_authors(self):
        comment = None
        if self.per_file:
            entry = self.per_file.where_filename(self.get_filename())
            if entry:
                comment = entry.filecomment.xml_text()
        if comment:
            check = Match(r"(?s)<para>\s*[Aa]uthors*\b:?"
                          r"((?:.(?!</para>))*.)</para>")
            if comment & check: return _email_to_xml(check[1])
            check = Match(r"(?m)(^|<para>)\s*@authors?\b:?(.*)")
            if comment & check: return _email_to_xml(check[2])
        return None
    def get_copyright(self):
        comment = None
        if self.per_file:
            entry = self.per_file.where_filename(self.get_filename())
            if entry:
                comment = entry.filecomment.xml_text()
        if comment:
            check = Match(r"(?s)<para>\s*[Cc]opyright\b"
                          r"((?:.(?!</para>))*.)</para>")
            if comment & check: return _email_to_xml(check[0])
        return None
class ForthWordPageAdapter:
    def __init__(self, exports, per_word):
        self.exports = exports
        self.per_word = per_word
        self.lookup = None
    def invalid(self):            return not self.exports.word
    def _word(self):              return self.exports.word
    def get_name(self):           return self._word().get_name()
    def get_param(self):          return self._word().get_param()
    def get_typedescriptor(self): return self._word().get_typedescriptor()
    def xml_name(self):           return _src_to_xml(self.get_name())
    def xml_param(self):          return _src_to_xml(self.get_param())
    def xml_typedescriptor(self): return _src_to_xml(self.get_typedescriptor())
    def _lookup(self):
        if self.lookup: return True
        name = self.get_name()
        self.lookup = self.per_word.where_name(name)
        return self.lookup is not None
    def xml_text(self):
        if not self._lookup(): return "<p>(no description)</p>"
        text = self.lookup.get_body().xml_text()
        # print self.get_name(), "=", text,"\n"
        if not text: return "<p>(?no description?)</p>"
        return _link_code(_hack_fixup(section2html(text)))
    def _head(self):
        if not self._lookup(): return None
        return self.lookup.get_head()
    def _body(self):
        if not self._lookup(): return None
        return self.lookup.get_body()
    def get_stack(self):
        head = self._head()
        if  head: return head.get_stack()
        return ""
    def xml_stack(self):        return _src_to_xml(self.get_stack())
    def get_hints(self):
        head = self._head()
        if  head: return head.get_hints()
        return ""
    def xml_hints(self):        return _src_to_xml(self.get_hints())
    def xml_wordlist(self):
        into = self.exports.into
        if into: return _src_to_xml(into)
        return ""

def makedocs(filenames, o):
    textfiles = []
    for filename in filenames:
        textfile = TextFile(filename)
        textfile.parse()
        textfiles += [ textfile ]
    per_file = PerFile()
    for textfile in textfiles:
        textfileheader = TextFileHeader(textfile)
        textfileheader.parse()
        filecomment = CommentMarkup(textfileheader)
        filecomment.parse()
        per_file.add(textfileheader, filecomment)
    funcheaders = [] #
    for textfile in per_file.textfileheaders:
        funcheader = FunctionHeaderList(textfile)
        funcheader.parse()
        funcheaders += [ funcheader ]
    per_function = PerFunction()
    for funcheader in funcheaders:
        for child in funcheader.get_children():
            funcprototype = FunctionPrototype(child)
            funcprototype.parse()
            funccomment = CommentMarkup(child)
            funccomment.parse()
            per_function.add(child, funccomment, funcprototype)
    per_family = PerFunctionFamily()
    for item in per_function.entries:
        per_family.add_PerFunctionEntry(item)
    per_family.fill_families()
    forthlists = [] #
    per_wordset = PerForthWordset()
    for textfile in per_file.textfileheaders:
        forthlist = ForthHeaderList(textfile)
        forthlist.parse()
        forthlists += [ forthlist ]
        wordsetlist = ForthWordsetList(textfile)
        wordsetlist.parse()
        for wordset in wordsetlist.get_children():
            wordset.parse()
            for exports in wordset.get_entries():
                word = ForthWord(exports)
                if word.parse(): exports.word = word
            per_wordset.add(wordset)
    per_forthword = PerForthWord()
    for headerlist in forthlists:
        for header in headerlist.get_children():
            notation = ForthNotation(header.get_notation(), header)
            comment = CommentMarkup(header)
            per_forthword.add(header, comment, notation)
    # debug output....
    # per_file.print_list_mainheader()
    # per_function.print_list_titleline()
    # per_function.print_list_name()
    # per_family.print_list_name()
    # per_forthword.print_list_name()
    #
    htmls = []
    for entry in per_wordset.entries:
        wordset = entry.wordset
        html = ForthWordsetHtmlPage(wordset, o)
        for exports in wordset.get_entries():
            html.add(ForthWordPageAdapter(exports, per_forthword))
        htmls += [ html ]
    htmldoc = HtmlDocument(o)
    htmldoc.title = o.package+" Forth Wordsets"
    for html in htmls:
        htmldoc.add(html)
    htmldoc.save("pfe-wordsets"+o.suffix)
    htmldocs = HtmlDirectory(o)
    htmldocs.title = o.package+" Forth Wordsets"
    for html in htmls:
        htmldocs.add(html)
    htmldocs.save("pfe-wordsets"+o.suffix)
    #
    html = FunctionListHtmlPage(o)
    for item in per_family.entries:
        for func in item.functions:
            func_adapter = HtmlManualPageAdapter(func)
            if o.onlymainheader and not (Match("<"+o.onlymainheader+">")
                                         & func_adapter.src_mainheader()):
                    continue
            html.add(func_adapter)
        html.cut()
    html.cut()
    class _Html_:
        def __init__(self, html):
            self.html = html
        def html_text(self):
            return section2html(paramdef2html(self.html.xml_text()))
        def get_title(self):
            return self.html.get_title()
    HtmlDocument(o).add(_Html_(html)).save("pfe-words"+o.suffix)
    #
    man3 = FunctionListReference(o)
    for item in per_family.entries:
        for func in item.functions:
            func_adapter = RefEntryManualPageAdapter(func, per_file)
            man3.add(func_adapter)
        man3.cut()
    man3.cut()
    DocbookDocument(o).add(man3).save("pfe-words"+o.suffix)
    
if __name__ == "__main__":
    filenames = []
    o = Options()
    o.suffix = ""
    o.package = "PFE/Forth"
    o.program = sys.argv[0]
    o.html = "html"
    o.docbook = "docbook"
    for item in sys.argv[1:]:
        if o.scan(item): continue
        filenames += [ item ]
    try:
        makedocs(filenames, o)
    except Exception, e:
        print "BYE", e
