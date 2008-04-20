from match import Match

forthmodulevariablenames = [ "title", "text", "version", "author", "see", "comment" ]

class ForthModuleEntry:
    def __init__(self, textfile, infoblock, comment):
        self.textfile = textfile
        self.infoblock = infoblock
        self.comment = comment
        self.variables = {}
        for name in forthmodulevariablenames:
            self.variables[name] = ""
        self.parse_infoblock(self.infoblock)
    def get_infoblock(self):
        return self.infoblock
    def get_comment(self):
        return self.comment
    def get_filename(self):
        return self.textfile.get_filename()
    def parse_infoblock(self, text):
        name = ""
        newtitle = Match(r"\s?[*]\s\s?--\s(.*)")
        newsection = Match(r"\s?[*]\s\s?@(\w+)\s+(.*)")
        emptyline = Match(r"\s?[*]?\s*$")
        indentline = Match(r"\s?[*]?\s?\s?(.*)$")
        for line in str(text).splitlines():
            if newtitle & line and not self.variables["title"]:
                self.variables["title"] = newtitle.group(1)
                name = "text"
            elif newsection & line:
                name = newsection.group(1)
                self.variables[name] = newsection.group(2)
            else:
                if not name and not emptyline & line: 
                    name = "comment"
                if name:
                    if indentline & line:
                        self.variables[name] += "\n"+indentline.group(1)
                    else:
                        self.variables[name] += "\n"+line

class ForthModule:
    """ scan and parse the comment block at the start of a forth module implementation.
    That one has a standard format - it uses @-notation to mark version and author
    and the last @description section is in the common => ForthNotation syntax """
    def __init__(self, textfile = None):
        self.textfile = textfile # TextFile
        self.children = None     # src'style
    def parse(self, textfile = None):
        if textfile is not None:
            self.textfile = textfile
        if self.textfile is None:
            return False
        text = self.textfile.get_src_text()
        m = Match(r"(?s)(?:\/\*[*]+\s+"
                  r"((?:.(?!\*\/))*.)@description"
                  r"((?:.(?!\*\/))*.)\*\/\s*)")
        self.children = []
        for found in m.finditer(text):
            infoblock = found.group(1)
            comment = found.group(2)
            child = ForthModuleEntry(self.textfile, infoblock, comment)
            self.children += [ child ]
        return len(self.children) > 0
    def get_filename(self):
        return self.textfile.get_filename()
    def get_children(self):
        if self.children is None:
            if not self.parse(): return []
        return self.children

if __name__ == "__main__":
    import sys
    from textfile import TextFile, _src_to_xml as xml
    from commentmarkup import CommentMarkup
    parser = ForthModule()
    started = False
    for filename in sys.argv[1:]:
        if filename.startswith("-"):
            _help(sys.argv[0])
        else:
            parser.parse(TextFile(filename))
            if not started:
                print '<!DOCUMENTTYPE forthmodulexml SYSTEM "forth.dtd">'
                print "<forthmodulexml>"
                started = True
            print ' <forthmodulelist>'
            for info in parser.get_children():
                print '  <forthmodule filename="%s">' % xml(info.get_filename())
                print '   <forthmoduleinfo>'
                for name in forthmodulevariablenames:
                    value = info.variables[name]
                    print '    <%s>%s</%s>' % (name, xml(value), name)
                for name in info.variables.keys():
                    if name not in forthmodulevariablenames:
                        print '    <value name="%s">%s</value>' % (name, xml(value))
                print '   </forthmodulinfo>'
                markup = CommentMarkup()
                markup.parse(info)
                print '   <comment>%s</comment>' % markup.xml_text()
                print '  </forthmodule>'
            print ' </forthmodulelist>'
    if started:
        print '<forthmodulexml>'
        
                                