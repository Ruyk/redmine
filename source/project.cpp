#include <config.h>
#include <http.h>
#include <project.h>
#include <util.h>

#include <json/json.hpp>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iterator>
#include <sstream>

namespace redmine {
project::project()
    : id(),
      name(),
      identifier(),
      description(),
      homepage(),
      created_on(),
      updated_on(),
      parent() {}

result project::init(const json::object &object) {
  auto Name = object.get("name");
  CHECK_JSON_PTR(Name, json::TYPE_STRING);
  name = Name->string();

  auto Id = object.get("id");
  CHECK_JSON_PTR(Id, json::TYPE_NUMBER);
  id = Id->number<uint32_t>();

  auto Identifier = object.get("identifier");
  CHECK_JSON_PTR(Identifier, json::TYPE_STRING);
  identifier = Identifier->string();

  auto Description = object.get("description");
  CHECK_JSON_PTR(Description, json::TYPE_STRING);
  description = Description->string();

  auto CreatedOn = object.get("created_on");
  CHECK_JSON_PTR(CreatedOn, json::TYPE_STRING);
  created_on = CreatedOn->string();

  auto UpdatedOn = object.get("updated_on");
  CHECK_JSON_PTR(UpdatedOn, json::TYPE_STRING);
  updated_on = UpdatedOn->string();

  auto Parent = object.get("parent");
  if (Parent) {
    CHECK_JSON_TYPE((*Parent), json::TYPE_OBJECT);

    auto Name = Parent->object().get("name");
    CHECK_JSON_PTR(Name, json::TYPE_STRING);
    parent.name = Name->string();

    auto Id = Parent->object().get("id");
    CHECK_JSON_PTR(Id, json::TYPE_NUMBER);
    parent.id = Id->number<uint32_t>();
  }

  return SUCCESS;
}

json::object project::jsonify() const {
  UNREACHABLE("redmine::project::jsonify() not implemented!");
  return json::object();
}

bool project::operator==(const project &other) const { return id == other.id; }

bool project::operator==(const char *str) const {
  const uint32_t Id = std::strtoul(str, nullptr, 0);
  if (id == Id || name == str || identifier == str) {
    return true;
  }
  return false;
}

namespace action {
result project(int argc, char **argv, options options) {
  if (0 == argc) {
    fprintf(stderr,
            "usage: redmine project <action> [args]\n"
            "actions:\n"
            "        list\n"
            "        new <name> <identifier>\n"
            "        show <show>\n");
    return FAILURE;
  }

  if (!strcmp("list", argv[0])) {
    return project_list(argc - 1, argv + 1, options);
  }

  if (!strcmp("new", argv[0])) {
    return project_new(argc - 1, argv + 1, options);
  }

  if (!strcmp("show", argv[0])) {
    return project_show(argc - 1, argv + 1, options);
  }

  fprintf(stderr, "invalid argument: %s\n", argv[0]);
  return INVALID_ARGUMENT;
}

result project_list(int argc, char **argv, options options) {
  CHECK(0 != argc, fprintf(stderr, "invalid argument: %s\n", argv[0]);
        return INVALID_ARGUMENT);

  // TODO: Support cached project list for command line completion.

  redmine::config config;
  CHECK(config.load(), fprintf(stderr, "invalid config file\n");
        return INVALID_CONFIG);

  std::vector<redmine::project> projects;
  CHECK_RETURN(query::projects(config, options, projects));

  // TODO: Display information about the output of the fields below?
  printf(
      "  id | identifier                        | name\n"
      "-----|-----------------------------------|------------------------------"
      "--------\n");
  for (auto &project : projects) {
    int32_t numSpaces = 34 - project.identifier.size();
    std::vector<char> spaces((0 > numSpaces) ? 0 : numSpaces, ' ');
    spaces.back() = '\0';
    printf("%4u | %s%s | %s\n", project.id, project.identifier.c_str(),
           spaces.data(), project.name.c_str());
  }

  return SUCCESS;
}

result project_new(int argc, char **argv, options options) {
  CHECK(2 > argc,
        fprintf(stderr, "usage: redmine project new <name> <identifier>\n");
        return FAILURE);
  if (2 < argc) {
    fprintf(stderr, "invalid arguments:");
    for (int argi = 0; argi < argc; ++argi) {
      fprintf(stderr, " %s\n", argv[argi]);
    }
    return FAILURE;
  };

  redmine::config config;
  CHECK(config.load(), fprintf(stderr, "invalid config file\n");
        return INVALID_CONFIG);

  std::string filename = util::getcwd();
  filename += "/REDMINE_NEW_PROJECT";
  CHECK(has<DEBUG>(options), printf("%s\n", filename.c_str()));
  {
    // NOTE: Populate the REDMINE_PROJECT_NEW temporary file
    std::ofstream file(filename);
    CHECK(!file.is_open(),
          fprintf(stderr, "could not create temporary file: %s\n",
                  filename.c_str());
          return FAILURE);
    file << "name: " << argv[0] << "\n";
    file << "identifier: " << argv[1] << "\n";
    file << "description: \n";
    file << "homepage: \n";
    file << "is_public: true\n";
    file << "parent_id: \n";  // the parent project number
    file << "inherit_members: false\n";
    file << "tracker_ids: \n";  // (repeatable element) the tracker id: 1 for
    // Bug, etc.
    file << "enabled_module_names: boards, calendar, documents, files, gantt, "
            "issue_tracking, news, repository, time_tracking, wiki\n";
  }

  // TODO: Make editor configureable
  std::string editor("vim");

  // TODO: Open REDMINE_PROJECT_NEW in editor
  std::string command = editor + " " + filename;
  int result = std::system(command.c_str());
  CHECK(result, fprintf(stderr, "failed to load editor: %s", editor.c_str());
        return FAILURE);

  std::stringstream content;
  {
    // NOTE: Read user content of REDMINE_PROJECT_NEW file
    std::ifstream file(filename);
    CHECK(!file.is_open(),
          fprintf(stderr, "could not open temporary file: %s\n",
                  filename.c_str());
          return FAILURE);
    std::copy(std::istreambuf_iterator<char>(file),
              std::istreambuf_iterator<char>(),
              std::ostreambuf_iterator<char>(content));
    // NOTE: Remove temporary file REDMINE_PROJECT_NEW
    util::rm(filename);
  }

  json::object project;

  // TODO: Validate content of REDMINE_PROJECT_NEW
  auto strip = [](const std::string &str, const char *prefix) -> std::string {
    size_t len = std::strlen(prefix);
    if (str.size() > len) {
      return str.substr(len, str.size());
    } else {
      return "";
    }
  };

  std::string name;
  std::getline(content, name);
  project.add("name", strip(name, "name: "));

  std::string identifier;
  std::getline(content, identifier);
  identifier = strip(identifier, "identifier: ");
  project.add("identifier", identifier);

  std::string description;
  std::getline(content, description);
  description = strip(description, "description: ");
  if (!description.empty()) {
    project.add("description", description);
  }

  std::string homepage;
  std::getline(content, homepage);
  homepage = strip(homepage, "homepage: ");
  if (!homepage.empty()) {
    project.add("homepage", homepage);
  }

  std::string is_public;
  std::getline(content, is_public);
  is_public = strip(is_public, "is_public: ");
  if (!is_public.empty()) {
    project.add("is_public", is_public);
  }

  std::string parent_id;
  std::getline(content, parent_id);
  parent_id = strip(parent_id, "parent_id: ");
  if (!parent_id.empty()) {
    project.add("parent_id", parent_id);
  }

  std::string inherit_members;
  std::getline(content, inherit_members);
  inherit_members = strip(inherit_members, "inherit_members: ");
  if (!inherit_members.empty()) {
    project.add("inherit_members", inherit_members);
  }

  std::string data = json::write(json::object("project", project));

  CHECK(has<DEBUG>(options), printf("%s\n", data.c_str()));
  std::string body;
  redmine::result error = http::post("/projects.json", config, options,
                                     http::code::CREATED, data, body);
  json::value root = json::read(body, false);
  if (error) {
    CHECK_JSON_TYPE(root, json::TYPE_OBJECT);
    json::value *errors = root.object().get("errors");
    CHECK_JSON_PTR(errors, json::TYPE_ARRAY);
    for (json::value &error : errors->array()) {
      CHECK_JSON_TYPE(error, json::TYPE_STRING);
      fprintf(stderr, "error: %s\n", error.string().c_str());
    }
  }

  // NOTE: Display result
  {
    printf("project created\n");
    CHECK(json::TYPE_OBJECT != root.type(), return FAILURE);

    auto project = root.object().get("project");
    CHECK(!project, return FAILURE);

    auto name = project->object().get("name");
    printf("name: %s\n", name->string().c_str());
    CHECK(!name, return FAILURE);

    auto id = project->object().get("id");
    CHECK(!id, return FAILURE);
    printf("id: %u\n", static_cast<uint32_t>(id->number()));

    auto identifier = project->object().get("identifier");
    CHECK(!identifier, return FAILURE);
    printf("identifier: %s\n", identifier->string().c_str());

    auto description = project->object().get("description");
    CHECK(!description, return FAILURE);
    printf("description: %s\n", description->string().c_str());

    auto homepage = project->object().get("homepage");
    CHECK(!homepage, return FAILURE);
    printf("homepage: %s\n", homepage->string().c_str());

    auto created_on = project->object().get("created_on");
    CHECK(!created_on, return FAILURE);
    printf("created on: %s\n", created_on->string().c_str());

    auto updated_on = project->object().get("updated_on");
    CHECK(!updated_on, return FAILURE);
    printf("updated on: %s\n", updated_on->string().c_str());
  }

  return SUCCESS;
}

result project_show(int argc, char **argv, options options) {
  CHECK(0 == argc, fprintf(stderr, "missing id or name\n"); return FAILURE);
  CHECK(1 < argc, fprintf(stderr, "invalid argument: %s\n", argv[1]);
        return FAILURE);

  redmine::config config;
  CHECK(config.load(), fprintf(stderr, "invalid config file\n");
        return INVALID_CONFIG);

  // TODO: Lookup projects for name and get id
  std::string id(argv[0]);

  std::string body;
  CHECK_RETURN(http::get(std::string("/projects/") + id + ".json", config,
                         options, body));

  json::value root = json::read(body, false);
  CHECK(has<DEBUG>(options), printf("%s\n", json::write(root, "  ").c_str()));

  auto &Project = root.object().get("project")->object();
  redmine::project project;
  CHECK_RETURN(project.init(Project));

  printf("       name: %s\n", project.name.c_str());
  printf("         id: %u\n", project.id);
  printf(" identifier: %s\n", project.identifier.c_str());
  printf("description: %s\n", project.description.c_str());
  if (!project.homepage.empty()) {
    printf("   homepage: %s\n", project.homepage.c_str());
  }
  printf(" created_on: %s\n", project.created_on.c_str());
  printf(" updated_on: %s\n", project.updated_on.c_str());
  if (!project.parent.name.empty()) {
    printf("parent name: %s\n", project.parent.name.c_str());
    printf("  parent id: %u\n", project.parent.id);
  }

  return SUCCESS;
}
}

result query::projects(config &config, options options,
                       std::vector<project> &projects) {
  std::string body;
  CHECK_RETURN(http::get("/projects.json", config, options, body));

  auto root = json::read(body, false);
  CHECK_JSON_TYPE(root, json::TYPE_OBJECT);

  CHECK(has<DEBUG>(options), printf("%s\n", json::write(root, "  ").c_str()));

  auto Projects = root.object().get("projects");
  CHECK_JSON_PTR(Projects, json::TYPE_ARRAY);

  for (auto Project : Projects->array()) {
    CHECK_JSON_TYPE(Project, json::TYPE_OBJECT);

    redmine::project project;
    CHECK_RETURN(project.init(Project.object()));

    projects.push_back(project);
  }

  return SUCCESS;
}
}
