#pragma once
#include <numeric>
#include <algorithm>
#include <functional>
#include <memory>
//#include "Engine.h"
#include "Hud.h"
#include "HudManager.h"
#include "Unit.h"
#include "Building.h"
#include "TaskManager.h"
#include "Assets.h"
#include "Projectiles.h"
#include "WorldObject.h"
#include "WorldObjectManager.h"
#include <queue>
/*

namespace olc
{
	struct rect
	{
		olc::vf2d pos;
		olc::vf2d size;

		rect(const olc::vf2d& p = { 0.0f, 0.0f }, const olc::vf2d& s = { 1.0f, 1.0f }) : pos(p), size(s)
		{

		}

		constexpr bool contains(const olc::vf2d& p) const
		{
			return !(p.x < pos.x || p.y < pos.y || p.x >= (pos.x + size.x) || p.y >= (pos.y + size.y));
		}

		constexpr bool contains(const olc::rect& r) const
		{
			return (r.pos.x >= pos.x) && (r.pos.x + r.size.x < pos.x + size.x) &&
				(r.pos.y >= pos.y) && (r.pos.y + r.size.y < pos.y + size.y);
		}

		constexpr bool overlaps(const olc::rect& r) const
		{
			return (pos.x < r.pos.x + r.size.x && pos.x + size.x >= r.pos.x && pos.y < r.pos.y + r.size.y && pos.y + size.y >= r.pos.y);
		}
	};

};


// Constrain depth of Quad Tree. Since its floating point, it could in principle sub-divide for
// a very long time, consuming far more time and memory than is sensible
constexpr size_t MAX_DEPTH = 8;


template <typename OBJECT_TYPE>
class StaticQuadTree
{
public:
	StaticQuadTree(const olc::rect& size = { {0.0f, 0.0f}, {100.0f, 100.0f} }, const size_t nDepth = 0)
	{
		m_depth = nDepth;
		resize(size);
	}

	// Force area change on Tree, invalidates this and all child layers
	void resize(const olc::rect& rArea)
	{
		// Erase this layer
		clear();

		// Recalculate area of children
		m_rect = rArea;
		olc::vf2d vChildSize = m_rect.size / 2.0f;

		// Cache child areas local to this layer
		m_rChild =
		{
			// Top Left
			olc::rect(m_rect.pos, vChildSize),
			// Top Right
			olc::rect({m_rect.pos.x + vChildSize.x, m_rect.pos.y}, vChildSize),
			// Bottom Left
			olc::rect({m_rect.pos.x, m_rect.pos.y + vChildSize.y}, vChildSize),
			// Bottom Right
			olc::rect(m_rect.pos + vChildSize, vChildSize)
		};

	}

	// Clears the contents of this layer, and all child layers
	void clear()
	{
		// Erase any items stored in this layer
		m_pItems.clear();

		// Iterate through children, erase them too
		for (int i = 0; i < 4; i++)
		{
			if (m_pChild[i])
				m_pChild[i]->clear();
			m_pChild[i].reset();
		}
	}

	// Returns a count of how many items are stored in this layer, and all children of this layer
	size_t size() const
	{
		size_t nCount = m_pItems.size();
		for (int i = 0; i < 4; i++)
			if (m_pChild[i]) nCount += m_pChild[i]->size();
		return nCount;
	}

	// Inserts an object into this layer (or appropriate child layer), given the area the item occupies
	void insert(const OBJECT_TYPE& item, const olc::rect& itemsize)
	{
		// Check each child
		for (int i = 0; i < 4; i++)
		{
			// If the child can wholly contain the item being inserted
			if (m_rChild[i].contains(itemsize))
			{
				// Have we reached depth limit?
				if (m_depth + 1 < MAX_DEPTH)
				{
					// No, so does child exist?
					if (!m_pChild[i])
					{
						// No, so create it
						m_pChild[i] = std::make_shared<StaticQuadTree<OBJECT_TYPE>>(m_rChild[i], m_depth + 1);
					}

					// Yes, so add item to it
					m_pChild[i]->insert(item, itemsize);
					return;
				}
			}
		}

		// It didnt fit, so item must belong to this quad
		m_pItems.push_back({ itemsize, item });
	}

	// Returns a list of objects in the given search area
	std::list<OBJECT_TYPE> search(const olc::rect& rArea) const
	{
		std::list<OBJECT_TYPE> listItems;
		search(rArea, listItems);
		return listItems;
	}

	// Returns the objects in the given search area, by adding to supplied list
	void search(const olc::rect& rArea, std::list<OBJECT_TYPE>& listItems) const
	{
		// First, check for items belonging to this area, add them to the list
		// if there is overlap
		for (const auto& p : m_pItems)
		{
			if (rArea.overlaps(p.first))
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
				if (rArea.contains(m_rChild[i]))
					m_pChild[i]->items(listItems);

				// If child overlaps with search area then checks need
				// to be made
				else if (m_rChild[i].overlaps(rArea))
					m_pChild[i]->search(rArea, listItems);
			}
		}
	}

	void items(std::list<OBJECT_TYPE>& listItems) const
	{
		// No questions asked, just return child items
		for (const auto& p : m_pItems)
			listItems.push_back(p.second);

		// Now add children of this layer's items
		for (int i = 0; i < 4; i++)
			if (m_pChild[i]) m_pChild[i]->items(listItems);
	}


	std::list<OBJECT_TYPE> items() const
	{
		// No questions asked, just return child items
		std::list<OBJECT_TYPE> listItems;
		items(listItems);
		return listItems;
	}

	// Returns area of this layer
	const olc::rect& area()
	{
		return m_rect;
	}


protected:
	// Depth of this StaticQuadTree layer
	size_t m_depth = 0;

	// Area of this StaticQuadTree
	olc::rect m_rect;

	// 4 child areas of this StaticQuadTree
	std::array<olc::rect, 4> m_rChild{};

	// 4 potential children of this StaticQuadTree
	std::array<std::shared_ptr<StaticQuadTree<OBJECT_TYPE>>, 4> m_pChild{};

	// Items which belong to this StaticQuadTree
	std::vector<std::pair<olc::rect, OBJECT_TYPE>> m_pItems;
};


template <typename OBJECT_TYPE>
class StaticQuadTreeContainer
{
	// Using a std::list as we dont want pointers to be invalidated to objects stored in the
	// tree should the contents of the tree change
	using QuadTreeContainer = std::list<OBJECT_TYPE>;

protected:
	// The actual container
	QuadTreeContainer m_allItems;

	// Use our StaticQuadTree to store "pointers" instead of objects - this reduces
	// overheads when moving or copying objects 
	StaticQuadTree<typename QuadTreeContainer::iterator> root;

public:
	StaticQuadTreeContainer(const olc::rect& size = { {0.0f, 0.0f}, { 100.0f, 100.0f } }, const size_t nDepth = 0) : root(size, nDepth)
	{

	}

	// Sets the spatial coverage area of the quadtree
	// Invalidates tree
	void resize(const olc::rect& rArea)
	{
		root.resize(rArea);
	}

	// Returns number of items within tree
	size_t size() const
	{
		return m_allItems.size();
	}

	// Returns true if tree is empty
	bool empty() const
	{
		return m_allItems.empty();
	}

	// Removes all items from tree
	void clear()
	{
		root.clear();
		m_allItems.clear();
	}


	// Convenience functions for ranged for loop
	typename QuadTreeContainer::iterator begin()
	{
		return m_allItems.begin();
	}

	typename QuadTreeContainer::iterator end()
	{
		return m_allItems.end();
	}

	typename QuadTreeContainer::const_iterator cbegin()
	{
		return m_allItems.cbegin();
	}

	typename QuadTreeContainer::const_iterator cend()
	{
		return m_allItems.cend();
	}


	// Insert item into tree in specified area
	void insert(const OBJECT_TYPE& item, const olc::rect& itemsize)
	{
		// Item is stored in container
		m_allItems.push_back(item);

		// Pointer/Area of item is stored in quad tree
		root.insert(std::prev(m_allItems.end()), itemsize);
	}

	// Returns a std::list of pointers to items within the search area
	std::list<typename QuadTreeContainer::iterator> search(const olc::rect& rArea) const
	{
		std::list<typename QuadTreeContainer::iterator> listItemPointers;
		root.search(rArea, listItemPointers);
		return listItemPointers;
	}

};

*/

class UnitManager {
    std::vector<std::weak_ptr<Unit>> unitList; // copy of just units

    void addNewUnit(std::weak_ptr<Unit> unit); // world manager use only

    std::vector<std::weak_ptr<Unit>> selectedUnits; // units currently selected - DO NOT USE EXTERNALY
public:
    UnitManager();
    ~UnitManager() = default;

    void Update(float delta);
    void CollectGarbage();
	
	TaskManager taskMgr;

	void DelegateTask(const std::string& name, const std::any& data);
	std::shared_ptr<Unit> This_shared_pointer(olc::vf2d pos);
	void StopUnits();
	void MoveConstructBuilding(const std::string& buildingName, const olc::vf2d& Target);
    void MoveUnits(olc::vf2d Target , bool Attackstate);
	void SelectUnit(olc::vf2d Mouse);
    void SelectUnits(olc::vf2d Initial, olc::vf2d Final);
	//prob a better way
	std::shared_ptr<Collidable> FindObject(olc::vf2d Mouse);
	void ParseObject(std::shared_ptr<Collidable> object, std::shared_ptr<Building>& build, std::shared_ptr<Unit>& unit );
	
	std::vector<std::shared_ptr<Collidable>> testobjects;

	std::vector<std::shared_ptr<Collidable>> FindObjects(olc::vf2d pos,float Radius);
	void ParseObjects(std::vector<std::shared_ptr<Collidable>> object, std::queue<std::weak_ptr<Building>>& build, std::queue<std::weak_ptr<Unit>>& unit);

	std::shared_ptr<Collidable> SearchClosestObject(olc::vf2d pos, float Radius);
	void ParseSearch(std::shared_ptr<Collidable> object, std::weak_ptr<Building>& build, std::weak_ptr<Unit>& unit);

	void DeselectUnits();

	size_t GetSelectedUnitCount();
    size_t GetUnitCount(const std::string& name="");
    std::shared_ptr<Unit> GetUnit(const std::string& name, size_t index=0);
    std::shared_ptr<Unit> GetUnit(size_t index=0);

	bool IterateSelectedUnits(std::function<bool(std::shared_ptr<Unit>)> cb);
	bool IterateAllUnits(std::function<bool(std::shared_ptr<Unit>)> cb);

    friend class WorldManager;
	friend class TaskManager;

};