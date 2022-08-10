#pragma once
#include "types.h"

#include <vector>

namespace HoleMap {

// Track class stores all observations of a 3D point.
struct TrackElement {
  TrackElement();
  TrackElement(const uint32_t image_id, const uint32_t point2D_idx);
  // The image in which the track element is observed.
  uint32_t image_id;
  // The point in the image that the track element is observed.
  uint32_t point2D_idx;
};

class Track {
 public:
  Track();

  // The number of track elements.
  inline size_t Length() const;

  // Access all elements.
  inline const std::vector<TrackElement>& Elements() const;
  inline std::vector<TrackElement>& Elements();
  inline void SetElements(const std::vector<TrackElement>& elements);

  // Access specific elements.
  inline const TrackElement& Element(const size_t idx) const;
  inline TrackElement& Element(const size_t idx);
  inline void SetElement(const size_t idx, const TrackElement& element);

  // Append new elements.
  inline void AddElement(const TrackElement& element);
  inline void AddElement(const uint32_t image_id, const uint32_t point2D_idx);
  inline void AddElements(const std::vector<TrackElement>& elements);

  // Delete existing element.
  inline void DeleteElement(const size_t idx);
  void DeleteElement(const uint32_t image_id, const uint32_t point2D_idx);

  // Requests that the track capacity be at least enough to contain the
  // specified number of elements.
  inline void Reserve(const size_t num_elements);

  // Shrink the capacity of track vector to fit its size to save memory.
  inline void Compress();

 private:
  std::vector<TrackElement> elements_;
};

////////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////////

size_t Track::Length() const { return elements_.size(); }

const std::vector<TrackElement>& Track::Elements() const { return elements_; }

std::vector<TrackElement>& Track::Elements() { return elements_; }

void Track::SetElements(const std::vector<TrackElement>& elements) {
  elements_ = elements;
}

// Access specific elements.
const TrackElement& Track::Element(const size_t idx) const {
  return elements_.at(idx);
}

TrackElement& Track::Element(const size_t idx) { return elements_.at(idx); }

void Track::SetElement(const size_t idx, const TrackElement& element) {
  elements_.at(idx) = element;
}

void Track::AddElement(const TrackElement& element) {
  elements_.push_back(element);
}

void Track::AddElement(const uint32_t image_id, const uint32_t point2D_idx) {
  elements_.emplace_back(image_id, point2D_idx);
}

void Track::AddElements(const std::vector<TrackElement>& elements) {
  elements_.insert(elements_.end(), elements.begin(), elements.end());
}

void Track::DeleteElement(const size_t idx) {
  elements_.erase(elements_.begin() + idx);
}

void Track::Reserve(const size_t num_elements) {
  elements_.reserve(num_elements);
}

void Track::Compress() { elements_.shrink_to_fit(); }

}