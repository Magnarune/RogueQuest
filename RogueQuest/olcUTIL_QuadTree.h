#pragma once

#include "olcPixelGameEngine.h"
#include "olcUTIL_Geometry2D.h"



namespace olc::utils
{
	template <typename T, typename CTYPE = float>
	struct QuadTreeItemLocation
	{
		typename std::list<std::pair<geom2d::rect<CTYPE>, T>>* container = nullptr;
		typename std::list<std::pair<geom2d::rect<CTYPE>, T>>::iterator iterator;
	};

	template <typename pT, typename CTYPE = float>
	class DynamicQuadTree
	{
	public:
		DynamicQuadTree(const geom2d::rect<CTYPE>& size, const size_t nDepth = 0, const size_t nMaxDepth = 8)
		{
			m_depth = nDepth;
			m_rect = size;
			m_maxdepth = nMaxDepth;
			resize(m_rect);
		}

		// Insert a region into this area
		QuadTreeItemLocation<pT> insert(const pT item, const geom2d::rect<CTYPE>& itemsize)
		{
			for (int i = 0; i < 4; i++)
			{
				if (geom2d::contains(m_rChild[i], itemsize))
				{
					// Have we reached depth limit?
					if (m_depth + 1 < m_maxdepth)
					{
						// No, so does child exist?
						if (!m_pChild[i])
						{
							// No, so create it
							m_pChild[i] = std::make_shared<DynamicQuadTree<pT>>(m_rChild[i], m_depth + 1, m_maxdepth);
						}

						// Yes, so add item to it
						return m_pChild[i]->insert(item, itemsize);
					}
				}
			}

			// It didnt fit, so item must belong to this geom2d::rect<CTYPE>
			m_pItems.push_back({ itemsize, item });
			return { &m_pItems, std::prev(m_pItems.end()) };
		}

		void relocate(pT item, const geom2d::rect<CTYPE>& rArea)
		{
			// Remove it
			remove(item);

			// Reinsert it with new location
			insert(item, rArea);
		}

		size_t size() const
		{
			size_t nCount = m_pItems.size();
			for (int i = 0; i < 4; i++)
				if (m_pChild[i]) nCount += m_pChild[i]->size();
			return nCount;
		}

		void search(const geom2d::rect<CTYPE>& rArea, std::list<pT>& listItems) const
		{
			// First, check for items belonging to this area, add them to the list
			// if there is overlap
			for (const auto& p : m_pItems)
			{
				if (geom2d::overlaps(rArea, p.first))
					listItems.push_back(p.second);
			}

			// Second, recurse through children and see if they can
			// add to the list
			for (int i = 0; i < 4; i++)
			{
				if (m_pChild[i])
				{
					// If child is entirely contained within area, recursively
					// add all of its children, no need to check boundaries
					if (geom2d::contains(rArea, m_rChild[i]))
						m_pChild[i]->items(listItems);

					// If child overlaps with search area then checks need
					// to be made
					else if (geom2d::overlaps(m_rChild[i], rArea))
						m_pChild[i]->search(rArea, listItems);
				}
			}


		}

		void items(std::list<pT>& listItems) const
		{
			// No questions asked, just return child items
			for (const auto& p : m_pItems)
				listItems.push_back(p.second);

			for (int i = 0; i < 4; i++)
				if (m_pChild[i]) m_pChild[i]->items(listItems);
		}

		void clear()
		{
			m_pItems.clear();

			for (int i = 0; i < 4; i++)
			{
				if (m_pChild[i])
					m_pChild[i]->clear();
				else
					m_pChild[i].reset();
			}
		}

		void resize(const geom2d::rect<CTYPE>& rArea)
		{
			clear();
			m_rect = rArea;
			olc::v2d_generic<CTYPE> vChildSize = m_rect.size / CTYPE(2);
			m_rChild =
			{
				geom2d::rect<CTYPE>(m_rect.pos, vChildSize),
				geom2d::rect<CTYPE>({m_rect.pos.x + vChildSize.x, m_rect.pos.y}, vChildSize),
				geom2d::rect<CTYPE>({m_rect.pos.x, m_rect.pos.y + vChildSize.y}, vChildSize),
				geom2d::rect<CTYPE>(m_rect.pos + vChildSize, vChildSize)
			};
		}


		const geom2d::rect<CTYPE>& area()
		{
			return m_rect;
		}

	protected:
		size_t m_depth = 0;
		size_t m_maxdepth = 10;

		// Area of this quadnode
		geom2d::rect<CTYPE> m_rect;

		// 4 child areas of this quadnode
		std::array<geom2d::rect<CTYPE>, 4> m_rChild{};

		// 4 potential children of this quadnode
		std::array<std::shared_ptr<DynamicQuadTree<pT>>, 4> m_pChild{};

		// Items which belong to this quadnode
		std::list<std::pair<geom2d::rect<CTYPE>, pT>> m_pItems;

	};

	template<typename T, typename CTYPE = float>
	struct QuadTreeItem
	{
		// The item Itself
		T item;

		// A "locator" to the container/iterator that points to this item's iterator in the
		// top level list - phew
		QuadTreeItemLocation<typename std::list<QuadTreeItem<T, CTYPE>>::iterator> pItem;
	};

	template <typename T, typename CTYPE = float>
	class QuadTreeContainer
	{
		using IQuadtreeContainer = std::list<QuadTreeItem<T, CTYPE>>;

	public:
		QuadTreeContainer(const geom2d::rect<CTYPE>& size = { {0.0f, 0.0f}, { 100.0f, 100.0f } }, const size_t nDepth = 0, const size_t nMaxDepth = 8) : root(size, nDepth, nMaxDepth)
		{

		}

		// Sets the spatial coverage area of teh quadtree
		void resize(const geom2d::rect<CTYPE>& rArea)
		{
			root.resize(rArea);
		}

		// Inserts an item into the quadtree
		void insert(const T& item, const geom2d::rect<CTYPE>& itemsize)
		{
			QuadTreeItem<T> newItem;
			newItem.item = item;

			// Item i stored in container
			m_allItems.emplace_back(newItem);

			// Pointer/Area of item is stored in geom2d::rect<CTYPE> tree
			m_allItems.back().pItem = root.insert(std::prev(m_allItems.end()), itemsize);
		}

		// Returns a std::list of pointers to items within the search area
		std::list<typename IQuadtreeContainer::iterator> search(const geom2d::rect<CTYPE>& rArea) const
		{
			std::list<typename IQuadtreeContainer::iterator> listItemPointers;
			root.search(rArea, listItemPointers);
			return listItemPointers;
		}

		void remove(typename IQuadtreeContainer::iterator& item)
		{
			// Iterator points to a QuadTreeItem
			item->pItem.container->erase(item->pItem.iterator);

			// Remove actual item from container
			m_allItems.erase(item);
		}

		void relocate(typename IQuadtreeContainer::iterator& item, const geom2d::rect<CTYPE>& itemsize)
		{
			// Remove pointer to item from whichever container its stored in
			item->pItem.container->erase(item->pItem.iterator);

			// Update the items pointer by reinsertion into geom2d::rect<CTYPE> tree
			item->pItem = root.insert(item, itemsize);

		}

		typename IQuadtreeContainer::iterator begin()
		{
			return m_allItems.begin();
		}

		typename IQuadtreeContainer::iterator end()
		{
			return m_allItems.end();
		}

		typename IQuadtreeContainer::const_iterator cbegin()
		{
			return m_allItems.cbegin();
		}

		typename IQuadtreeContainer::const_iterator cend()
		{
			return m_allItems.cend();
		}

		size_t size() const
		{
			return root.size();
		}

		void clear()
		{
			root.clear();
		}

		const geom2d::rect<CTYPE>& area()
		{
			return root.area();
		}

		IQuadtreeContainer m_allItems;
	protected:
		DynamicQuadTree<typename IQuadtreeContainer::iterator> root;
	};
}
