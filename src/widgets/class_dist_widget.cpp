#include "class_dist_widget.hpp"

#include "../edit_moderator.hpp"
#include "../project.hpp"

#include <implot.h>

#include <map>

#include <cmath>

void
class_dist_widget::render(edit_moderator<project>& proj)
{
  if (m_dirty) {

    update_distribution(proj.get());

    m_dirty = false;
  }

  std::vector<const char*> labels(m_names.size());

  for (std::size_t i = 0; i < m_names.size(); i++) {
    labels[i] = m_names[i].c_str();
  }

  if (ImPlot::BeginPlot("Class Distribution", ImVec2(-1, -1), ImPlotFlags_Equal | ImPlotFlags_NoTitle)) {

    ImPlot::PlotPieChart(labels.data(), m_weights.data(), m_weights.size(), 0, 0, 1);

    ImPlot::EndPlot();
  }
}

void
class_dist_widget::on_project_modification(const project& /* proj */)
{
  m_dirty = true;
}

void
class_dist_widget::update_distribution(const project& proj)
{
  std::map<int, float> counter;

  std::map<int, std::string> names;

  for (const auto& ent : proj.class_defs) {

    names.emplace(ent.value, ent.name);

    counter.emplace(ent.value, 0.0f);
  }

  for (const auto& f : proj.frames) {
    if (f.class_id) {
      counter.at(f.class_id.value()) += 1.0f;
    }
  }

  float max{ 0.0f };

  for (const auto& c : counter) {
    max += c.second;
  }

  m_names.clear();

  m_weights.clear();

  for (const auto& n : names) {
    m_names.emplace_back(n.second);
  }

  for (const auto& v : counter) {
    m_weights.emplace_back(v.second / max);
  }
}
