from match import Match

def _unpack(text):
    quotedstring = Match("\"([.*])\"")
    if text & quotedstring:
        return quotedstring[1] & Match(r"\\(.)") >> "\\1"
    return text

class ForthWordsetEntry:
    def __init__(self, wordset, typeco, export, into, comment):
        self.wordset = wordset # parent
        self.typeco = typeco
        self.export = export
        self.into = into
        self.comment = comment
    def get_typeco(self):
        return self.typeco
    def get_export(self):
        return self.export
    def get_into(self):
        return self.into
    def get_comment(self):
        return self.comment
    def get_filename(self):
        return self.wordset.get_filename()
    def get_wordset_name(self):
        return self.wordset.get_wordset_name()
    def get_wordset_hint(self):
        return self.wordset.get_wordset_hint()

class ForthWordset:
    """ parsing the comment block that shall contain a forth
    stack notation line plus a description block - the stack notation is
    passed along for further parsing through => ForthNotation """
    def __init__(self, forthwordsetlist, listname, exports, wordset, comment):
        self.parent = forthwordsetlist
        self.listname = listname
        self.exports = exports
        self.wordset = wordset
        self.comment = comment
        self.wordset_name = None
        self.wordset_hint = None
        self.entries = []
    def get_filename(self):
        return self.parent.get_filename()
    def parse(self):
        pass
    def parse_wordset(self, wordset = None):
        if wordset is not None:
            self.wordset = wordset
        wordset = self.wordset
        if wordset is None:
            return False
        found = Match("\"([\w-]+) ([^\"]*)\"")
        if wordset & found:
            self.wordset_name = found[1]
            self.wordset_hint = found[2]
            return True
        return False
    def _parsed_wordset(self):
        if not self.wordset_name:
            if not self.parse_wordset(): return False
        return True
    def get_wordset_name(self):
        if not self._parsed_wordset(): return None
        return self.wordset_name
    def get_wordset_hint(self):
        if not self._parsed_wordset(): return None
        return self.wordset_hint
    def get_listname(self):
        return self.listname
    def get_comment(self):
        return self.comment
    def get_entries(self):
        if not len(self.entries):
            self.parse_exports()
        return self.entries
    def parse_exports(self):
        into_wordlist = "[FORTH]"
        export_hint = ""
        for line in self.exports.split("\n"):
            into = Match(r"\s*(P4_INTO)"
                         r"\s\(\s*(\".*\")\s*,\s*(\w+)\)")
            expo = Match(r"\s*(P4_FXco|P4_IXco|P4_SXco)"
                         r"\s*\(\s*(.*)\)")
            hint = Match(r"\/\*((?:.(!?\*\/))*.)\*\/")
            if line & into:
                into_wordlist = _unpack(into[2])
                continue
            if line & hint:
                export_hint = hint[1]
                continue
            if line & expo:
                self.entries += [ ForthWordsetEntry(self, expo[1], expo[2],
                                                    into_wordlist,
                                                    export_hint) ]
                continue
        return True
    def parse(self):
        self.parse_wordset()
        self.parse_exports()
    
class ForthWordsetList:
    """ scan for forth wordset export table in the source file that will
    usually start with P4_LISTWORDS and end with P4_COUNTWORDS.
    Unpack the export lines and fill a list of children. """
    def __init__(self, textfile = None):
        self.textfile = textfile # TextFile
        self.children = None     # src'style
    def parse(self, textfile = None):
        if textfile is not None:
            self.textfile = textfile
        if self.textfile is None:
            return False
        text = self.textfile.get_src_text()
        m = Match(r"(?s)(?:\/\*[*]+(?=\s)"
                  r"((?:.(?!\*\/))*.)\*\/\s*)?"
                  r"P4_LISTWORDS\s*\(\s*(\w+)\s*\)\s*="
                  r"\s*\{((?:.(?!P4_COUNTWORDS))*)\};"
                  r"\s*P4_COUNTWORDS\s*\(\s*(\w+)\s*,\s*([^\(\)]*)\)")
        self.children = []
        for found in m.finditer(text):
            comment = found.group(1)
            listname = found.group(2)
            exports = found.group(3)
            listcheck = found.group(4)
            wordset = found.group(5)
            if listname != listcheck:
                print "listname '"+listname+"' <> '"+listcheck+"'"
                continue
            child = ForthWordset(self, listname, exports, wordset, comment)
            self.children += [ child ]
        return len(self.children) > 0
    def get_filename(self):
        return self.textfile.get_filename()
    def get_children(self):
        if self.children is None:
            if not self.parse(): return []
        return self.children
