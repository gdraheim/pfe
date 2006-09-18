from match import Match

class ForthHeader:
    """ parsing the comment block that shall contain a forth
    stack notation line plus a description block - the stack notation is
    passed along for further parsing through => ForthNotation """
    def __init__(self, forthheaderlist, comment):
        self.parent = forthheaderlist
        self.comment = comment
        self.firstline = None
        self.otherlines = None
        self.stackline = None
        self.alsolist = []
    def get_filename(self):
        return self.parent.get_filename()
    def parse_firstline(self):
        if not self.comment: return False
        x = self.comment.find("\n")
        if x > 0:
            self.firstline = self.comment[:x]
            self.otherlines = self.comment[x:]
        elif x == 0:
            self.firstline = "..."
            self.otherlines = self.comment[1:x]
        else:
            self.firstline = self.comment
            self.otherlines = ""
        return True
    def get_firstline(self):
        if self.firstline is None:
            if not self.parse_firstline(): return ""
        return self.firstline
    def get_otherlines(self):
        if self.firstline is None:
            if not self.parse_firstline(): return ""
        return self.otherlines
    def parse_stackline(self):
        """ split extra-notes from the firstline - keep only stackline """
        line = self.get_firstline()
        if line is None: return False
        self.stackline = line
        self.alsolist = []
        x = line.find("also:")
        if x > 0:
            self.stackline = line[:x]
            for also in line[x+5:].split(","):
                self.alsolist += [ also.strip() ]
            print "[[[", self.alsolist, "}}}", self.stackline
        self._alsolist = self.alsolist
        return True
    def get_alsolist(self):
        """ gets the see-also notes from the firstline """
        if self.stackline is None:
            if not self.parse_stackline(): return None
        return self.alsolist
    def get_stackline(self):
        """ gets firstline with see-also notes removed """
        if self.stackline is None:
            if not self.parse_stackline(): return False
        return self.stackline
    def get_notation(self):
        """ gets notation stackline unless that is a redirect """
        stackline = self.get_stackline()
        if stackline & Match(r"^\s*=>"): return ""
        if stackline & Match(r"^\s*<link>"): return ""
        return stackline
    def firstline_has_stackline_notation(self):
        firstline = self.get_firstline()
        if not firstline: return False
        return firstline & Match(r"\([^\(\)]*--[^\(\)]*\)")
    
class ForthHeaderList:
    """ scan for comment blocks in the source file that have a firstline
    something quite like a Forth stack effect notation (stackchanger).
    Unpack the descriptions and fill self.comment and self.prototype. """
    def __init__(self, textfile = None):
        self.textfile = textfile # TextFile
        self.children = None     # src'style
    def parse(self, textfile = None):
        if textfile is not None:
            self.textfile = textfile
        if self.textfile is None:
            return False
        text = self.textfile.get_src_text()
        m = Match(r"(?s)\/\*[*]+(?=\s)"
                  r"((?:.(?!\*\/))*.)\*\/")
        self.children = []
        for found in m.finditer(text):
            child = ForthHeader(self, found.group(1))
            if child.firstline_has_stackline_notation():
                self.children += [ child ]
        return len(self.children) > 0
    def get_filename(self):
        return self.textfile.get_filename()
    def get_children(self):
        if self.children is None:
            if not self.parse(): return []
        return self.children
