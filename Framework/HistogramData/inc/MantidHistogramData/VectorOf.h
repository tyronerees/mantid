#ifndef MANTID_HISTOGRAMDATA_VECTOROF_H_
#define MANTID_HISTOGRAMDATA_VECTOROF_H_

#include "MantidHistogramData/DllConfig.h"
#include "MantidKernel/cow_ptr.h"
#include "MantidKernel/make_cow.h"

namespace Mantid {
namespace HistogramData {
namespace detail {

/** VectorOf

  This class is an implementation detail of classes like HistogramData::BinEdges
  and HistogramData::Points. It wraps a copy-on-write pointer to an underlying
  data type based on std::vector, such as HistogramX and HistogramY.

  The first template parameter is the type of the inheriting class. This is the
  CRTP (curiously recurring template pattern).

  The second template parameter is the type of the object to store.

  @author Simon Heybrock
  @date 2016

  Copyright &copy; 2016 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
  National Laboratory & European Spallation Source

  This file is part of Mantid.

  Mantid is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  Mantid is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  File change history is stored at: <https://github.com/mantidproject/mantid>
  Code Documentation is available at: <http://doxygen.mantidproject.org>
*/
template <class T, class CowType> class VectorOf {
public:
  /// Default constructor, the stored object will be NULL.
  VectorOf() = default;
  /// Construct stored object of length count initialized to the constant value.
  VectorOf(size_t count, const double &value) {
    m_data = Kernel::make_cow<CowType>(count, value);
  }
  /// Construct stored object of length count.
  explicit VectorOf(size_t count) { m_data = Kernel::make_cow<CowType>(count); }
  /// Construct stored object with the contents of the initializer list init.
  VectorOf(std::initializer_list<double> init) {
    m_data = Kernel::make_cow<CowType>(init);
  }
  /// Copy constructor. Lightweight, stored object will be shared.
  VectorOf(const VectorOf &) = default;
  /// Move constructor.
  VectorOf(VectorOf &&) = default;
  // Note the lvalue reference qualifier for all assignment operators. This
  // prevents mistakes in client code, assigning to an rvalue, such as
  // histogram.getBinEdges() = { 0.1, 0.2 };
  /// Copy assignment. Lightweight, stored object will be shared.
  VectorOf &operator=(const VectorOf &)& = default;
  /// Move assignment.
  VectorOf &operator=(VectorOf &&)& = default;

  /// Assigns the stored object with the contents of the initializer list init.
  VectorOf &operator=(std::initializer_list<double> ilist) & {
    m_data = Kernel::make_cow<CowType>(ilist);
    return *this;
  }
  /// Constructs the stored object with the contents of the range [first, last).
  template <class InputIt>
  VectorOf(InputIt first, InputIt last)
      : m_data(Kernel::make_cow<CowType>(first, last)) {}

  /// Copy construct from cow_ptr. Lightweight, stored object will be shared.
  explicit VectorOf(const Kernel::cow_ptr<CowType> &other) : m_data(other) {}
  /// Copy construct from shared_ptr. Lightweight, stored object will be shared.
  explicit VectorOf(const boost::shared_ptr<CowType> &other) : m_data(other) {}
  /// Copy construct stored object from data.
  explicit VectorOf(const CowType &data)
      : m_data(Kernel::make_cow<CowType>(data)) {}
  /// Move construct stored object from data.
  explicit VectorOf(CowType &&data)
      : m_data(Kernel::make_cow<CowType>(std::move(data))) {}

  /// Copy assignment from cow_ptr. Lightweight, stored object will be shared.
  VectorOf &operator=(const Kernel::cow_ptr<CowType> &other) & {
    m_data = other;
    return *this;
  }
  /// Copy assignment from shared_ptr. Lightweight, stored object will be
  /// shared.
  VectorOf &operator=(const boost::shared_ptr<CowType> &other) & {
    m_data = other;
    return *this;
  }
  /// Copy assignment to stored object from data.
  VectorOf &operator=(const CowType &data) & {
    if (!m_data || (&(*m_data) != &data))
      m_data = Kernel::make_cow<CowType>(data);
    return *this;
  }
  /// Move assignment to stored object from data.
  VectorOf &operator=(CowType &&data) & {
    if (!m_data || (&(*m_data) != &data))
      m_data = Kernel::make_cow<CowType>(std::move(data));
    return *this;
  }

  /// Checks if *this stores a non-null pointer.
  explicit operator bool() const { return m_data.operator bool(); }

  /// Returns true if the stored object has size 0. The behavior is undefined if
  /// the stored pointer is null.
  bool empty() const { return m_data->empty(); }

  /// Returns the size of the stored object. The behavior is undefined if the
  /// stored pointer is null.
  size_t size() const { return m_data->size(); }

  /// Returns a const reference to the stored object. The behavior is undefined
  /// if the stored pointer is null.
  const CowType &data() const { return *m_data; }
  /// Returns a reference to the stored object. The behavior is undefined if the
  /// stored pointer is null.
  CowType &mutableData() { return m_data.access(); }
  /// Returns a copy-on-write pointer to the stored object.
  Kernel::cow_ptr<CowType> cowData() const { return m_data; }
  /// Returns a const reference to the internal data structure of the stored
  /// object. The behavior is undefined if the stored pointer is null.
  const std::vector<double> &rawData() const { return m_data->rawData(); }
  /// Returns a reference to the internal data structure of the stored object.
  /// The behavior is undefined if the stored pointer is null.
  std::vector<double> &mutableRawData() {
    return m_data.access().mutableRawData();
  }

protected:
  // This is used as base class only, cannot delete polymorphically, so
  // destructor is protected.
  ~VectorOf() = default;

  Kernel::cow_ptr<CowType> m_data{nullptr};
};

} // namespace detail
} // namespace HistogramData
} // namespace Mantid

#endif /* MANTID_HISTOGRAMDATA_VECTOROF_H_ */
