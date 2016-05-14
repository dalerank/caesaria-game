function FileSystem() {
}

FileSystem.prototype.getFiles = function(dir, ext) {
  return g_session.getFiles(dir, ext);
}

FileSystem.prototype.getPath = function (path) {
  var rpath = new Path();
  rpath.set(path);
  return rpath;
}

var g_fs = new FileSystem();
