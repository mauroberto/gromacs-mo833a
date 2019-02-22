/*
 * This file is part of the GROMACS molecular simulation package.
 *
 * Copyright (c) 2019, by the GROMACS development team, led by
 * Mark Abraham, David van der Spoel, Berk Hess, and Erik Lindahl,
 * and including many others, as listed in the AUTHORS file in the
 * top-level source directory and at http://www.gromacs.org.
 *
 * GROMACS is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * GROMACS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GROMACS; if not, see
 * http://www.gnu.org/licenses, or write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA.
 *
 * If you want to redistribute modifications to GROMACS, please
 * consider that scientific software is very special. Version
 * control is crucial - bugs must be traceable. We will be happy to
 * consider code for inclusion in the official distribution, but
 * derived work must not be called official GROMACS. Details are found
 * in the README & COPYING files - if they are missing, get the
 * official version at http://www.gromacs.org.
 *
 * To help us fund GROMACS development, we humbly ask that you cite
 * the research papers on the package. Check out http://www.gromacs.org.
 */
/*! \internal \file
 * \brief
 * Implements test of exclusionblock routines
 *
 * \author Paul Bauer <paul.bauer.q@gmail.com>
 * \ingroup module_topology
 */
#include "gmxpre.h"

#include "gromacs/topology/exclusionblocks.h"

#include <gtest/gtest.h>

#include "gromacs/topology/block.h"
#include "gromacs/utility/arrayref.h"
#include "gromacs/utility/smalloc.h"

#include "testutils/cmdlinetest.h"

namespace gmx
{
namespace testing
{
namespace
{

//! Add a new group to t_blocka
void addGroupToBlocka(t_blocka *b, gmx::ArrayRef<const int> index)
{
    srenew(b->index, b->nr+2);
    srenew(b->a, b->nra+index.size());
    for (int i = 0; (i < index.ssize()); i++)
    {
        b->a[b->nra++] = index[i];
    }
    b->nr++;
    b->index[b->nr] = b->nra;
}

//! Fill ExclusionBlock with data.
int fillExclusionBlock(ExclusionBlocks *b)
{
    srenew(b->nra, b->nr);
    srenew(b->a, b->nr);
    std::vector < std::vector < int>> index = {{0, 4, 7}, {1, 5, 8, 10}, {2, 6, 9, 11, 12}};
    int nra = 0;
    for (int i = 0; i < b->nr; i++)
    {
        srenew(b->a[i], index[i].size());
        for (unsigned j = 0; j < index[i].size(); j++)
        {
            b->a[i][j] = index[i][j];
        }
        b->nra[i] = index[i].size();
        nra      += b->nra[i];
    }
    return nra;
}
//! Fill the t_blocka with some datastructures
void makeTestBlockAData(t_blocka *ba)
{
    init_blocka(ba);

    std::vector<int> index = {12, 11, 9, 6, 2};
    addGroupToBlocka(ba, index);
    index = {10, 8, 5, 1};
    addGroupToBlocka(ba, index);
    index = {7, 4, 0};
    addGroupToBlocka(ba, index);
}

class ExclusionBlockTest : public ::testing::Test
{
    public:
        ExclusionBlockTest()
        {
            const int natom = 3;
            makeTestBlockAData(&ba_);
            initExclusionBlocks(&b_, natom);
        }
        ~ExclusionBlockTest() override
        {
            doneExclusionBlocks(&b_);
            done_blocka(&ba_);
        }

        void compareBlocks()
        {
            for (int i = 0; i < b_.nr; i++)
            {
                int index  = ba_.index[i];
                for (int j = 0; j < b_.nra[i]; j++)
                {
                    int pos = index + j;
                    EXPECT_EQ(b_.a[i][j], ba_.a[pos])<< "Block mismatch at " << i << " , " << j << ".";
                }
            }
        }
    protected:

        t_blocka        ba_;
        ExclusionBlocks b_;
};

TEST_F(ExclusionBlockTest, ConvertBlockAToExclusionBlocks)
{
    blockaToExclusionBlocks(&ba_, &b_);
    compareBlocks();
}

TEST_F(ExclusionBlockTest, ConvertExclusionBlockToBlocka)
{
    int             nra = fillExclusionBlock(&b_);
    srenew(ba_.a, nra+1);
    srenew(ba_.index, b_.nr+1);
    exclusionBlocksToBlocka(&b_, &ba_);
    compareBlocks();
}

TEST_F(ExclusionBlockTest, MergeExclusions)
{
    mergeExclusions(&ba_, &b_);
    compareBlocks();
}

}  // namespace

}  // namespace testing

}  // namespace gmx